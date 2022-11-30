#pragma once
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "async_task/async_task.h"
#include "container/concurrent_map.h"
#include "taskflow/include/async_task/async_task.h"
#include "taskflow/include/container/concurrent_map.h"

using std::string;

class Task {
 public:
  explicit Task(const std::string& task_name) : task_name_(task_name) {}
  Task(std::string task_name, AnyFunc* job)
      : task_name_(task_name), job_(job) {}

 public:
  std::string GetTaskName() const { return task_name_; }
  int GetDependencyCount() const { return dependencies_.size(); }
  std::vector<Task*> GetDependencies() const { return dependencies_; }
  AnyFunc* GetJob() { return job_; }
  bool GetFlag() { return in_progress_; }

  void AddDependecy(Task* task) { dependencies_.emplace_back(task); }
  void SetFlag(bool flag) { in_progress_ = flag; }

 private:
  const std::string task_name_;
  AnyFunc* job_ = nullptr;
  std::vector<Task*> dependencies_;
  bool in_progress_ = false;
};

class TaskManager {
 public:
  explicit TaskManager(std::vector<Task*> tasks)
      : worker_nums_(4), tasks_(tasks) {}
  TaskManager(std::vector<Task*> tasks, int worker_nums)
      : worker_nums_(worker_nums), tasks_(tasks) {}

  bool Init();
  void Run();

 private:
  void BuildDependencyMap();
  bool CircleCheck();

 private:
  uint64_t worker_nums_;
  std::vector<Task*> tasks_;
  stone::ConcurrentMap<string, int> dependency_map_;
  stone::ConcurrentMap<string, std::vector<Task*>> dependend_map_;
  std::vector<std::shared_ptr<taskflow::TaskWorker>> workers_;
  stone::ConcurrentMap<string, int> map_finish_;
};
