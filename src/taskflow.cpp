// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include "taskflow/include/taskflow.h"

#include <sys/time.h>

#include "taskflow/include/json/json_parser.h"
#include "taskflow/include/utils/latency_guard.h"

using std::string;
using std::unordered_map;
using std::vector;

constexpr char kFuncPrefix[] = "func_";
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

void TaskManager::Run() {
  while (uint64_t(finish_num_.load()) != graph_->GetTasks().size()) {
    for (const auto& task : graph_->GetTasks()) {
      // 找出没有前置依赖并且还没执行的task
      if (atomic_predecessor_count_[task->GetTaskName()]->load() == 0 &&
          !map_in_progress_.find(task->GetTaskName())) {
        // 设置执行状态为true
        map_in_progress_.emplace(task->GetTaskName(), 1);
        // 构建task任务
        auto t = [this, task] {
          // 执行用户设定的task
          if (const auto func =
                  so_script_->GetFunc(kFuncPrefix + task->GetOpName());
              func != nullptr) {
            vector<std::string> input;
            for (const auto& each : task->GetPredecessors()) {
              input.emplace_back(each->GetTaskName());
            }
            input_context_->task_config[task->GetTaskName()] =
                task->GetTaskConfig();
            input_context_->task_output[task->GetTaskName()] =
                func(*input_context_, input, task->GetTaskName());
          } else {
            TASKFLOW_ERROR("func of {} is empty!", task->GetTaskName());
          }
          // 执行完之后，更新依赖此task任务的依赖数
          if (graph_->GetSuccessorMap()->find(task->GetTaskName())) {
            for (const auto& each :
                 graph_->GetSuccessorMap()->at(task->GetTaskName())) {
              atomic_predecessor_count_[each->GetTaskName()]->fetch_sub(1);
            }
          }

          // 更新finish数组
          finish_num_.fetch_add(1);
        };
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
  return s;
}

void TaskManager::Clear() {
  atomic_predecessor_count_.clear();
  map_in_progress_.clear();
  delete input_context_;
  graph_.reset();
}

}  // namespace taskflow
