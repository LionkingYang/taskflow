// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <any>
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "taskflow/include/utils/string_utils.h"

namespace taskflow {
class Task;
using TaskPtr = std::shared_ptr<Task>;
class Task {
 public:
  explicit Task(const string& task_name, const string& op_name,
                const string& config)
      : task_name_(task_name), op_name_(op_name) {
    config_map_ = taskflow::split_twice(config, "|", "=");
  }
  const string& GetTaskName() const { return task_name_; }
  const string& GetOpName() const { return op_name_; }
  const std::unordered_map<std::string, std::string>& GetTaskConfig() const {
    return config_map_;
  }
  int GetPredecessorCount() const { return predecessors_.size(); }
  const vector<TaskPtr>& GetPredecessors() const { return predecessors_; }

  void AddPredecessor(TaskPtr task) { predecessors_.emplace_back(task); }

 private:
  const string task_name_;
  const string op_name_;
  std::unordered_map<std::string, std::string> config_map_;
  vector<TaskPtr> predecessors_;
};
}  // namespace taskflow
