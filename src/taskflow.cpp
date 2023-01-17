// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include "taskflow/include/taskflow.h"

#include <sys/time.h>

#include "taskflow/include/json/json_parser.h"
#include "taskflow/include/utils/latency_guard.h"
#include "taskflow/include/utils/string_utils.h"

using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

constexpr char kFuncPrefix[] = "func_";
constexpr char kConditionRes[] = "res:";
constexpr char kConditionEnv[] = "env:";
namespace taskflow {

TaskManager::TaskManager(std::shared_ptr<Graph> graph,
                         taskflow::SoScript* so_script, const std::any& input,
                         std::any* output) {
  graph_ = graph;
  so_script_ = so_script;
  input_context_ = new TaskContext(input, output);
  input_context_->Clear();
  for (const auto& each : graph_->GetPredecessorCount()) {
    std::shared_ptr<std::atomic_int> count =
        std::make_shared<std::atomic_int>(0);
    count->store(each.second);
    atomic_predecessor_count_.emplace(each.first, count);
  }
}

template <typename T>
static inline bool JudgeCondition(const string& condition, const T& param_res,
                                  const T& env_res) {
  if (taskflow::Contains(condition, ">=")) {
    return param_res >= env_res;
  } else if (taskflow::Contains(condition, "<=")) {
    return param_res <= env_res;
  } else if (taskflow::Contains(condition, ">")) {
    return param_res > env_res;
  } else if (taskflow::Contains(condition, "<")) {
    return param_res < env_res;
  } else {
    return param_res == env_res;
  }
}

bool TaskManager::OneCondition(const string& expr) {
  vector<string> params = taskflow::StrSplitByChars(expr, ">=<|", true);
  std::for_each(params.begin(), params.end(),
                [](string& s) { taskflow::TrimSpace(&s); });
  if (params.size() < 3) return false;
  const auto& env_key = params[0];
  const auto& env_value = params[1];
  const auto& env_type = params[2];
  if (!input_context_->task_env.find(env_key)) {
    TASKFLOW_ERROR("env key:{} not found", env_key);
    return false;
  }
  string env_res = input_context_->task_env.at(env_key);
  if (env_type == "int") {
    int int_res;
    int int_env_res;
    if (!absl::SimpleAtoi(params[1], &int_res)) return false;
    if (!absl::SimpleAtoi(env_res, &int_env_res)) return false;
    return JudgeCondition(expr, int_env_res, int_res);
  } else if (env_type == "double") {
    double double_res;
    double double_env_res;
    if (!absl::SimpleAtod(params[1], &double_res)) return false;
    if (!absl::SimpleAtod(env_res, &double_env_res)) return false;
    return JudgeCondition(expr, double_env_res, double_res);
  } else if (env_type == "float") {
    float float_res;
    float float_env_res;
    if (!absl::SimpleAtof(params[1], &float_res)) return false;
    if (!absl::SimpleAtof(env_res, &float_env_res)) return false;
    return JudgeCondition(expr, float_env_res, float_res);
  } else {
    return env_value == env_res;
  }
}

bool TaskManager::MatchCondition(const string& condition) {
  if (taskflow::HasPrefix(condition, kConditionEnv)) {
    string expr_str = condition.substr(4, condition.size() - 4);
    vector<string> exprs = taskflow::StrSplit(expr_str, "&&");
    std::for_each(exprs.begin(), exprs.end(),
                  [](string& s) { taskflow::TrimSpace(&s); });
    for (const auto& expr : exprs) {
      if (!OneCondition(expr)) {
        return false;
      }
    }
    return true;
  }
  return true;
}

void TaskManager::Run() {
  while (uint64_t(finish_num_.load()) != graph_->GetNodes().size()) {
    for (const auto& node : graph_->GetNodes()) {
      // 找出没有前置依赖并且还没执行的task
      const string& node_name = node->GetNodeName();
      if (atomic_predecessor_count_[node_name]->load() == 0 &&
          !map_in_progress_.find(node_name)) {
        // 设置执行状态为true
        map_in_progress_.emplace(node_name, 1);
        // 构建task任务
        auto done = [this, node] {
          // 执行完之后，更新依赖此task任务的依赖数

          for (const auto& each : node->GetSuccessors()) {
            atomic_predecessor_count_[each->GetNodeName()]->fetch_sub(1);
          }

          // 更新finish数组
          finish_num_.fetch_add(1);
        };
        if (switch_map_.find(node_name)) {
          done();
          continue;
        }
        auto task = node->GetTask();
        if (!task->GetCondition().empty() &&
            !MatchCondition(task->GetCondition())) {
          const auto& condition_map = graph_->GetConditionMap();
          if (const auto& iter = condition_map.find(node_name);
              iter != condition_map.end()) {
            for (const auto& node : iter->second) {
              switch_map_.emplace(node, 1);
            }
          }
          done();
          continue;
        }
        auto t = [this, node, task, done] {
          // 执行用户设定的task
          if (const auto func =
                  so_script_->GetFunc(kFuncPrefix + task->GetOpName());
              func != nullptr) {
            vector<std::string> input;
            for (const auto& each : node->GetPredecessors()) {
              input.emplace_back(each->GetNodeName());
            }
            input_context_->task_config[node->GetNodeName()] =
                task->GetTaskConfig();
            input_context_->task_output[node->GetNodeName()] =
                move(func(*input_context_, input, node->GetNodeName()));
          } else {
            TASKFLOW_ERROR("func of {} is empty!", node->GetNodeName());
          }
          // 非异步任务，执行完之后，更新依赖此task任务的依赖数
          if (!task->is_async()) done();
        };
        // 异步任务直接更新依赖关系
        if (task->is_async()) done();
        // 选取worker执行task
        taskflow::WorkManager::GetInstance()->Execute(t);
      }
    }
  }
}

std::string TaskManager::ToString() {
  std::string s;
  for (auto begin = atomic_predecessor_count_.begin();
       begin != atomic_predecessor_count_.end(); begin++) {
    taskflow::StrAppend(&s, begin->first, ":", begin->second->load(), "\n");
  }
  s += "graph:\n";
  s += graph_->ToString();
  return s;
}

void TaskManager::Clear() {
  atomic_predecessor_count_.clear();
  map_in_progress_.clear();
  delete input_context_;
  graph_.reset();
}

}  // namespace taskflow
