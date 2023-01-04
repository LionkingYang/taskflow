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
#include "taskflow/include/logger/logger.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/so_handler/so_handler.h"
#include "taskflow/include/utils/string_utils.h"
#include "taskflow/include/work_manager/work_manager.h"
using std::atomic;
using std::string;
using std::unordered_map;
using std::vector;

namespace taskflow {

class Task;
using TaskPtr = std::shared_ptr<Task>;
class Task {
 public:
  explicit Task(const string& task_name, const string& op_name, const string& config)
      : task_name_(task_name), op_name_(op_name) {
    config_map_ = taskflow::split_twice(config, "|", "=");
  }
  const string& GetTaskName() const { return task_name_; }
  const string& GetOpName() const { return op_name_; }
  const std::unordered_map<std::string, std::string>& GetTaskConfig() const { return config_map_; }
  int GetPredecessorCount() const { return predecessors_.size(); }
  const vector<TaskPtr>& GetPredecessors() const { return predecessors_; }

  void AddPredecessor(TaskPtr task) { predecessors_.emplace_back(task); }

 private:
  const string task_name_;
  const string op_name_;
  std::unordered_map<std::string, std::string> config_map_;
  vector<TaskPtr> predecessors_;
};

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
    return !GetCircle();
  }

  const taskflow::ConcurrentMap<string, vector<TaskPtr>>* GetSuccessorMap() { return &successor_map_; }
  taskflow::ConcurrentMap<string, int> GetPredecessorCount() { return predecessor_count_; }

  const vector<TaskPtr>& GetTasks() { return tasks_; }

  bool GetCircle() { return is_circle_; }

  std::string ToString();

 private:
  void BuildDependencyMap();
  bool BuildFromJson(const string& graph_path);
  void CircleCheck();

 private:
  taskflow::ConcurrentMap<string, int> predecessor_count_;
  taskflow::ConcurrentMap<string, vector<TaskPtr>> successor_map_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  vector<TaskPtr> tasks_;
  bool is_circle_ = false;
};

class TaskManager {
 public:
  // 使用已经建立好依赖关系的tasks列表进行初始化
  TaskManager(std::shared_ptr<Graph> graph, taskflow::SoScript* so_script, const std::any& input, std::any* output);
  void Run();
  ~TaskManager() { Clear(); }
  void Clear();

 private:
  void HandleTask(const taskflow::TaskPtr task);
  string ToString();

 private:
  std::shared_ptr<taskflow::WorkManager> work_manager_;
  taskflow::ConcurrentMap<string, int> predecessor_count_;
  std::shared_ptr<Graph> graph_;
  taskflow::SoScript* so_script_;
  taskflow::ConcurrentMap<string, int> map_in_progress_;
  TaskContext* input_context_;
  std::mutex mu_;
  std::atomic<int> finish_num_ = 0;
};
}  // namespace taskflow
