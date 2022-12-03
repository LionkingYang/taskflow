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
  taskflow::ConcurrentMap<string, std::any> task_output;
  TaskContext(const std::any& input, std::any* output)
      : global_input(input), global_output(output) {}
};

class Task;
using TaskFunc = std::function<void(TaskContext*)>;
using TaskPtr = std::shared_ptr<Task>;
class Task {
 public:
  explicit Task(const std::string& task_name) : task_name_(task_name) {}
  Task(std::string task_name, TaskFunc* job)
      : task_name_(task_name), job_(job) {}

  std::string GetTaskName() const { return task_name_; }
  int GetDependencyCount() const { return dependencies_.size(); }
  std::vector<TaskPtr> GetDependencies() const { return dependencies_; }
  TaskFunc* GetJob() { return job_; }

  void AddDependecy(TaskPtr task) { dependencies_.emplace_back(task); }
  void SetJob(TaskFunc* job) { job_ = job; }

 private:
  const std::string task_name_;
  TaskFunc* job_ = nullptr;
  std::vector<TaskPtr> dependencies_;
};

class Graph {
 public:
  explicit Graph(const std::string& graph_string,
                 std::unordered_map<std::string, TaskFunc*>* func_map) {
    BuildFromJson(graph_string, func_map);
    BuildDependencyMap();
  }
  const taskflow::ConcurrentMap<string, std::vector<TaskPtr>>*
  GetDependendMap() {
    return &dependend_map_;
  }
  const taskflow::ConcurrentMap<string, int> GetDependencyMap() {
    return dependency_map_;
  }

  const std::vector<TaskPtr> GetTasks() { return tasks_; }

  bool CircleCheck();

 private:
  void BuildDependencyMap();
  void BuildFromJson(const std::string& graph_string,
                     std::unordered_map<std::string, TaskFunc*>* func_map);

 private:
  taskflow::ConcurrentMap<string, int> dependency_map_;
  taskflow::ConcurrentMap<string, std::vector<TaskPtr>> dependend_map_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  std::vector<TaskPtr> tasks_;
};

class TaskManager {
 public:
  // 使用已经建立好依赖关系的tasks列表进行初始化
  explicit TaskManager(std::shared_ptr<Graph> graph, const std::any& input,
                       std::any* output, int worker_nums = 4)
      : worker_nums_(worker_nums),
        graph_(graph),
        input_context_(std::make_shared<TaskContext>(input, output)) {
    dependency_map_ = graph_->GetDependencyMap();
  }

  void Run();
  ~TaskManager() { Clear(); }

 private:
  void Clear();
  void Init();

 private:
  uint64_t worker_nums_;
  taskflow::ConcurrentMap<string, int> dependency_map_;
  std::shared_ptr<Graph> graph_;
  std::vector<std::shared_ptr<taskflow::TaskWorker>> workers_;
  taskflow::ConcurrentMap<string, int> map_finish_;
  taskflow::ConcurrentMap<string, int> map_in_progress_;
  std::shared_ptr<TaskContext> input_context_;
};
