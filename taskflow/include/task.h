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

class Task {
 public:
  explicit Task(const string& op_name, const string& config,
                const string& condition, bool async)
      : op_name_(op_name), condition_(condition), async_(async) {
    config_map_ = taskflow::split_twice(config, "|", "=");
  }
  const string& GetOpName() const { return op_name_; }
  const std::unordered_map<std::string, std::string>& GetTaskConfig() const {
    return config_map_;
  }
  const string& GetCondition() { return condition_; }
  bool is_async() { return async_; }

 private:
  const string op_name_;
  const string condition_;
  std::unordered_map<std::string, std::string> config_map_;
  bool async_;
};
using TaskPtr = std::shared_ptr<Task>;

class Node;
using NodePtr = std::shared_ptr<Node>;
class Node {
 public:
  Node(const string& node_name, TaskPtr task_ptr, uint64_t timeout)
      : node_name_(node_name), task_(task_ptr), timeout_(timeout) {}
  const string& GetNodeName() { return node_name_; }
  const TaskPtr GetTask() { return task_; }
  void AddPredecessor(NodePtr node) { predecessors_.push_back(node); }
  void AddSuccessor(NodePtr node) { successors_.push_back(node); }
  int GetPredecessorCount() const { return predecessors_.size(); }
  const vector<NodePtr>& GetPredecessors() const { return predecessors_; }
  const vector<NodePtr>& GetSuccessors() const { return successors_; }
  const uint64_t GetTimeout() const { return timeout_; }

 private:
  const string node_name_;
  vector<NodePtr> predecessors_;
  vector<NodePtr> successors_;
  TaskPtr task_ = nullptr;
  uint64_t timeout_;
};
}  // namespace taskflow
