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
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "taskflow/include/async_task/work_manager.h"
#include "taskflow/include/common_struct/task_struct.h"
#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/macros/macros.h"

using std::string;
using std::unordered_map;
using std::vector;

namespace taskflow {

struct TaskContext {
  const std::any global_input;
  std::any* global_output;
  taskflow::ConcurrentMap<string, std::any> task_output;
  TaskContext(const std::any& input, std::any* output)
      : global_input(input), global_output(output) {}
};

class Task;
using TaskFunc = std::function<void(TaskContext*)>;
using TaskPtr = std::shared_ptr<Task>;
class Task {
 public:
  explicit Task(const string& task_name) : task_name_(task_name) {}
  Task(string task_name, TaskFunc* job) : task_name_(task_name), job_(job) {}

  string GetTaskName() const { return task_name_; }
  int GetDependencyCount() const { return dependencies_.size(); }
  vector<TaskPtr> GetDependencies() const { return dependencies_; }
  TaskFunc* GetJob() { return job_; }

  void AddDependecy(TaskPtr task) { dependencies_.emplace_back(task); }
  void SetJob(TaskFunc* job) { job_ = job; }

 private:
  const string task_name_;
  TaskFunc* job_ = nullptr;
  vector<TaskPtr> dependencies_;
};

class Graph {
 public:
  explicit Graph(const string& graph_string,
                 unordered_map<string, TaskFunc*>* func_map) {
    BuildFromJson(graph_string, func_map);
    BuildDependencyMap();
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
  void BuildFromJson(const string& graph_string,
                     unordered_map<string, TaskFunc*>* func_map);

 private:
  taskflow::ConcurrentMap<string, int> dependency_map_;
  taskflow::ConcurrentMap<string, vector<TaskPtr>> dependend_map_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  vector<TaskPtr> tasks_;
};

class TaskManager {
 public:
  // 使用已经建立好依赖关系的tasks列表进行初始化
  explicit TaskManager(std::shared_ptr<Graph> graph, const std::any& input,
                       std::any* output, uint64_t worker_nums = 4)
      : work_manager_(std::make_shared<taskflow::WorkManager>(4)),
        graph_(graph),
        input_context_(std::make_shared<TaskContext>(input, output)) {
    dependency_map_ = graph_->GetDependencyMap();
  }

  void Run();
  ~TaskManager() { Clear(); }

 private:
  void Clear();

 private:
  std::shared_ptr<taskflow::WorkManager> work_manager_;
  taskflow::ConcurrentMap<string, int> dependency_map_;
  std::shared_ptr<Graph> graph_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  taskflow::ConcurrentMap<string, int> map_in_progress_;
  std::shared_ptr<TaskContext> input_context_;
};
}  // namespace taskflow
