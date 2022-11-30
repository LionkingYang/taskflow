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
      : tasks_(tasks), worker_nums_(4) {}
  TaskManager(std::vector<Task*> tasks, int worker_nums)
      : tasks_(tasks), worker_nums_(worker_nums) {}

  bool Init() {
    BuildDependencyMap();
    if (CircleCheck()) return false;
    while (workers_.size() < worker_nums_) {
      workers_.push_back(std::make_shared<taskflow::TaskWorker>());
    }
    return true;
  }

  void Run() {
    while (true) {
      for (const auto task : tasks_) {
        if (dependency_map_[task->GetTaskName()] == 0 &&
            !map_finish_.find(task->GetTaskName()) && !task->GetFlag()) {
          task->SetFlag(true);
          auto t = [this, task] {
            if (nullptr != task->GetJob()) {
              (*task->GetJob())();
            }
            for (auto each : dependend_map_[task->GetTaskName()]) {
              dependency_map_[each->GetTaskName()]--;
            }
            map_finish_.emplace(task->GetTaskName(), 1);
          };
          uint32_t cursor = random() % workers_.size();
          auto worker = workers_[cursor];
          worker->Post(t, false);
        }
      }
      if (map_finish_.size() == tasks_.size()) {
        std::cout << "All tasks have finished!\n";
        break;
      }
    }
  }

 private:
  void BuildDependencyMap() {
    for (auto task : tasks_) {
      dependency_map_[task->GetTaskName()] = task->GetDependencyCount();
      for (auto dependency : task->GetDependencies()) {
        if (dependend_map_.find(dependency->GetTaskName())) {
          dependend_map_[dependency->GetTaskName()].emplace_back(task);

        } else {
          std::vector<Task*> vec;
          vec.emplace_back(task);
          dependend_map_.emplace(dependency->GetTaskName(), vec);
        }
      }
    }
  }

  bool CircleCheck() {
    auto dependency_map = dependency_map_;
    auto dependend_map = dependend_map_;
    auto map_finish = map_finish_;
    while (true) {
      bool found = false;
      for (const auto task : tasks_) {
        if (dependency_map[task->GetTaskName()] == 0 &&
            !map_finish_.find(task->GetTaskName())) {
          for (auto each : dependend_map[task->GetTaskName()]) {
            dependency_map[each->GetTaskName()]--;
          }
          map_finish.emplace(task->GetTaskName(), 1);
          found = true;
        }
      }
      if (map_finish.size() == tasks_.size()) {
        return false;
      }
      if (!found) {
        return true;
      }
    }
  }

 private:
  int worker_nums_;
  std::vector<Task*> tasks_;
  stone::ConcurrentMap<string, int> dependency_map_;
  stone::ConcurrentMap<string, std::vector<Task*>> dependend_map_;
  std::vector<std::shared_ptr<taskflow::TaskWorker>> workers_;
  stone::ConcurrentMap<string, int> map_finish_;
};
