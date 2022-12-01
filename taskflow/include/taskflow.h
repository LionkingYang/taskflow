#pragma once
#include <unistd.h>

#include <any>
#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "async_task/async_task.h"
#include "container/concurrent_map.h"
#include "taskflow/include/async_task/async_task.h"
#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"

using std::string;

struct Job {
  std::string task_name;
  std::vector<string> dependencies;
  KCFG_DEFINE_FIELDS(task_name, dependencies)
};

struct Jobs {
  std::vector<Job> tasks;
  KCFG_DEFINE_FIELDS(tasks)
};

struct TaskContext {
  const std::any global_input;
  std::any* global_output;
  stone::ConcurrentMap<string, std::any> task_output;
  TaskContext(const std::any& input, std::any* output)
      : global_input(input), global_output(output) {}
};
using TaskFunc = std::function<void(TaskContext*)>;

class Task {
 public:
  explicit Task(const std::string& task_name) : task_name_(task_name) {}
  Task(std::string task_name, TaskFunc* job)
      : task_name_(task_name), job_(job), in_progress_(false) {}

  std::string GetTaskName() const { return task_name_; }
  int GetDependencyCount() const { return dependencies_.size(); }
  std::vector<std::shared_ptr<Task>> GetDependencies() const {
    return dependencies_;
  }
  TaskFunc* GetJob() { return job_; }
  bool GetFlag() { return in_progress_.load(); }

  void AddDependecy(std::shared_ptr<Task> task) {
    dependencies_.emplace_back(task);
  }
  void SetFlag(bool flag) { in_progress_.store(flag); }
  void SetJob(TaskFunc* job) { job_ = job; }

 private:
  const std::string task_name_;
  TaskFunc* job_ = nullptr;
  std::vector<std::shared_ptr<Task>> dependencies_;
  std::atomic<bool> in_progress_;
};

class TaskManager {
 public:
  // 使用已经建立好依赖关系的tasks列表进行初始化
  explicit TaskManager(std::vector<std::shared_ptr<Task>> tasks,
                       const std::any& input, std::any* output,
                       int worker_nums = 4)
      : worker_nums_(worker_nums),
        tasks_(tasks),
        input_context_(std::make_shared<TaskContext>(input, output)) {}
  // 使用json文件进行初始化
  TaskManager(const std::string& graph_string,
              std::unordered_map<std::string, TaskFunc*>* func_map,
              const std::any& input, std::any* output)
      : worker_nums_(4),
        input_context_(std::make_shared<TaskContext>(input, output)) {
    BuildFromJson(graph_string, func_map);
  }

  bool Init();
  void Run();
  void Clear();

 private:
  void BuildDependencyMap();
  bool CircleCheck();
  void BuildFromJson(const std::string& graph_string,
                     std::unordered_map<std::string, TaskFunc*>* func_map);

 private:
  uint64_t worker_nums_;
  std::vector<std::shared_ptr<Task>> tasks_;
  stone::ConcurrentMap<string, int> dependency_map_;
  stone::ConcurrentMap<string, std::vector<Task*>> dependend_map_;
  std::vector<std::shared_ptr<taskflow::TaskWorker>> workers_;
  stone::ConcurrentMap<string, int> map_finish_;
  std::shared_ptr<TaskContext> input_context_;
};
