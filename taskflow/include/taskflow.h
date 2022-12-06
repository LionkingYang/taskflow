// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <unistd.h>

#include <any>
#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "oneapi/tbb/concurrent_queue.h"
#include "oneapi/tbb/task_group.h"
#include "taskflow/include/common_struct/task_struct.h"
#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/so_handler/so_handler.h"
#include "taskflow/include/work_manager/work_manager.h"
using std::string;
using std::unordered_map;
using std::vector;

namespace taskflow {

class Task;
using TaskPtr = std::shared_ptr<Task>;
class Task {
 public:
  explicit Task(const string& task_name) : task_name_(task_name) {}
  string GetTaskName() const { return task_name_; }
  int GetDependencyCount() const { return dependencies_.size(); }
  vector<TaskPtr> GetDependencies() const { return dependencies_; }

  void AddDependecy(TaskPtr task) { dependencies_.emplace_back(task); }

 private:
  const string task_name_;
  vector<TaskPtr> dependencies_;
};

class Graph {
 public:
  Graph() {}
  bool Init(const string& graph_path) {
    dependency_map_.clear();
    dependend_map_.clear();
    map_finish_.clear();
    BuildFromJson(graph_path);
    BuildDependencyMap();
    return !CircleCheck();
  }

  const taskflow::ConcurrentMap<string, vector<TaskPtr>>* GetDependendMap() {
    return &dependend_map_;
  }
  const taskflow::ConcurrentMap<string, int> GetDependencyMap() {
    return dependency_map_;
  }

  const vector<TaskPtr> GetTasks() { return tasks_; }

  bool CircleCheck();

 private:
  void BuildDependencyMap();
  void BuildFromJson(const string& graph_path);

 private:
  taskflow::ConcurrentMap<string, int> dependency_map_;
  taskflow::ConcurrentMap<string, vector<TaskPtr>> dependend_map_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  vector<TaskPtr> tasks_;
};

class TaskManager {
 public:
  // 使用已经建立好依赖关系的tasks列表进行初始化
  TaskManager() {}

  void Init(std::shared_ptr<Graph> graph, taskflow::SoScript* so_script,
            const std::any& input, std::any* output, uint64_t worker_nums = 4);
  void Run();
  ~TaskManager() { Clear(); }
  void Clear();

 private:
  void HandleTask(const taskflow::TaskPtr task);

 private:
  std::shared_ptr<taskflow::WorkManager> work_manager_;
  taskflow::ConcurrentMap<string, int> dependency_map_;
  std::shared_ptr<Graph> graph_;
  taskflow::SoScript* so_script_;
  taskflow::ConcurrentMap<string, int> map_in_progress_;
  TaskContext* input_context_;
  std::mutex mu_;
  std::atomic<int> finish_num_ = 0;
};
}  // namespace taskflow
