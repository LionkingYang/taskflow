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

#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/task.h"

using std::unordered_map;
using std::vector;
using taskflow::Task;
using taskflow::TaskPtr;

namespace taskflow {
class Graph {
 public:
  Graph() {}
  bool Init(const string& graph_path) {
    is_circle_ = false;
    predecessor_count_.clear();
    successor_map_.clear();
    map_finish_.clear();
    tasks_.clear();
    if (!BuildFromJson(graph_path)) {
      return false;
    }
    BuildDependencyMap();
    CircleCheck();
    return true;
  }

  const taskflow::ConcurrentMap<string, vector<TaskPtr>>* GetSuccessorMap() {
    return &successor_map_;
  }
  const std::unordered_map<string, int>& GetPredecessorCount() {
    return predecessor_count_;
  }

  const vector<TaskPtr>& GetTasks() { return tasks_; }

  bool GetCircle() { return is_circle_; }

  std::string ToString();

 private:
  void BuildDependencyMap();
  bool BuildFromJson(const string& graph_path);
  void CircleCheck();

 private:
  unordered_map<string, int> predecessor_count_;
  taskflow::ConcurrentMap<string, vector<TaskPtr>> successor_map_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  vector<TaskPtr> tasks_;
  bool is_circle_ = false;
};

}  // namespace taskflow
