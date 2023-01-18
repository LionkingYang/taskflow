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
  int i = 0;
  for (const auto& node : graph_->GetNodes()) {
    std::shared_ptr<std::atomic_int> count =
        std::make_shared<std::atomic_int>(0);
    count->store(node->GetPredecessors().size());
    predecessor_count_array_.push_back(count);
    index_map_.emplace(node->GetNodeName(), i++);
    input_context_->task_config[node->GetNodeName()] =
        node->GetTask()->GetTaskConfig();
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
  taskflow::TrimSpaceOfVector(&params);
  if (unlikely(params.size() < 3)) return false;
  const auto& env_key = params[0];
  const auto& env_value = params[1];
  const auto& env_type = params[2];
  if (unlikely(!input_context_->task_env.find(env_key))) {
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
  if (likely(taskflow::HasPrefix(condition, kConditionEnv))) {
    string expr_str = condition.substr(4, condition.size() - 4);
    vector<string> exprs = taskflow::StrSplit(expr_str, "&&");
    taskflow::TrimSpaceOfVector(&exprs);
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
  std::vector<std::future<void>> results;
  for (int i = 0; i < graph_->GetNodes().size(); i++) {
    auto node = graph_->GetNodes()[i];
    auto task_func = [this, node, i]() {
      // 等待前置任务完成
      while (predecessor_count_array_[i]->load() != 0) {
        std::this_thread::yield();
      }
      // 任务完成后更新依赖数
      auto done = [this, node] {
        // 执行完之后，更新依赖此task任务的依赖数
        for (const auto& each : node->GetSuccessors()) {
          predecessor_count_array_[index_map_.at(each->GetNodeName())]
              ->fetch_sub(1);
        }
      };
      // 任务被关闭，直接退出
      if (switch_map_.find(node->GetNodeName())) {
        done();
        return;
      }
      // 任务执行条件判断
      auto task = node->GetTask();
      if (!task->GetCondition().empty() &&
          !MatchCondition(task->GetCondition())) {
        const auto& condition_map = graph_->GetConditionMap();
        if (const auto& iter = condition_map.find(node->GetNodeName());
            iter != condition_map.end()) {
          for (const auto& node_name : iter->second) {
            switch_map_.emplace(node_name, 1);
          }
        }
        done();
        return;
      }
      // 执行用户设定的task
      auto t = [node, task, done, this] {
        auto func = so_script_->GetFunc(kFuncPrefix + task->GetOpName());
        if (unlikely(func == nullptr)) {
          TASKFLOW_ERROR("func of {} is empty!", node->GetNodeName());
          return;
        }
        vector<std::string> input(node->GetPredecessors().size());
        for (int i = 0; i < node->GetPredecessors().size(); i++) {
          input[i] = node->GetPredecessors()[i]->GetNodeName();
        }
        input_context_->task_output[node->GetNodeName()] =
            move(func(*input_context_, input, node->GetNodeName()));
      };
      // 异步任务先更新依赖关系
      if (task->is_async()) {
        done();
        t();
      } else {
        t();
        done();
      }
      return;
    };
    // 异步任务放进专门的等待队列
    if (!node->GetTask()->is_async()) {
      results.push_back(graph_->GetWorker()->Execute(task_func));
    } else {
      aync_results_.push_back(graph_->GetWorker()->Execute(task_func));
    }
  }
  // 等待图执行结束
  for (auto& result : results) {
    result.get();
  }
}

std::string TaskManager::ToString() {
  std::string s;
  taskflow::StrAppend(&s, "graph:\n");
  taskflow::StrAppend(&s, graph_->ToString());
  return s;
}

void TaskManager::Clear() {
  for (auto& result : aync_results_) {
    result.get();
  }
  delete input_context_;
  graph_.reset();
  index_map_.clear();
  predecessor_count_array_.clear();
}

}  // namespace taskflow
