// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include "taskflow/include/taskflow.h"

#include <sys/time.h>

#include "taskflow/include/utils/latency_guard.h"

using std::string;
using std::unordered_map;
using std::vector;
long getCurrentTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
namespace taskflow {

TaskManager::TaskManager(std::shared_ptr<Graph> graph,
                         taskflow::SoScript* so_script, const std::any& input,
                         std::any* output) {
  work_manager_ = std::make_shared<taskflow::WorkManager>();
  graph_ = graph;
  so_script_ = so_script;
  input_context_ = new TaskContext(input, output);
  input_context_->Clear();
  dependency_map_ = graph_->GetDependencyMap();
}

void TaskManager::Run() {
  while (true) {
    for (const auto& task : graph_->GetTasks()) {
      // 找出没有前置依赖并且还没执行的task
      if (dependency_map_[task->GetTaskName()] == 0 &&
          !map_in_progress_.find(task->GetTaskName())) {
        // 设置执行状态为true
        map_in_progress_.emplace(task->GetTaskName(), 1);
        // 构建task任务
        auto t = [this, task] {
          // 执行用户设定的task
          if (const auto func =
                  so_script_->GetFunc("func_" + task->GetTaskName());
              func != nullptr) {
            func(*input_context_);
          } else {
            TASKFLOW_ERROR("func of {} is empty!", task->GetTaskName());
          }
          // 执行完之后，更新依赖此task任务的依赖数
          if (graph_->GetDependendMap()->find(task->GetTaskName())) {
            {
              std::lock_guard<std::mutex> lock(mu_);
              for (const auto& each :
                   graph_->GetDependendMap()->at(task->GetTaskName())) {
                dependency_map_[each->GetTaskName()]--;
              }
            }
          }
          // 更新finish数组
          finish_num_.fetch_add(1);
        };
        // 随机选取worker执行task
        // tg_.run(t);
        work_manager_->Execute(t);
      }
    }
    // 任务全部完成，退出
    if (uint64_t(finish_num_.load()) == graph_->GetTasks().size()) {
      break;
    }
  }
}

void Graph::BuildDependencyMap() {
  for (auto task : tasks_) {
    // 构建依赖map，即某个task如a依赖了哪些task
    // 当某个task的值为0时，说明task没有前置依赖，可以被执行了
    dependency_map_[task->GetTaskName()] = task->GetDependencyCount();
    // 构建被依赖map，即某个task被哪些任务依赖
    // 这个是为了方便某个任务执行完成之后，通知相应任务减少自己的前置依赖数
    for (auto dependency : task->GetDependencies()) {
      if (dependend_map_.find(dependency->GetTaskName())) {
        dependend_map_[dependency->GetTaskName()].emplace_back(task);

      } else {
        vector<TaskPtr> vec;
        vec.emplace_back(task);
        dependend_map_.emplace(dependency->GetTaskName(), vec);
      }
    }
  }
}

void Graph::CircleCheck() {
  // 为了不影响后续正常执行，复制出一份依赖关系出来
  auto dependency_map = dependency_map_;
  auto dependend_map = dependend_map_;
  auto map_finish = map_finish_;
  // 此处逻辑与执行类似，只是多了一层判断
  // 会判断依赖关系能否完全解耦，即所有任务都可以最终到达没有前置依赖的地步
  while (true) {
    bool found = false;
    for (const auto& task : tasks_) {
      if (dependency_map[task->GetTaskName()] == 0 &&
          !map_finish.find(task->GetTaskName())) {
        for (auto each : dependend_map[task->GetTaskName()]) {
          dependency_map[each->GetTaskName()]--;
        }
        map_finish.emplace(task->GetTaskName(), 1);
        found = true;
      }
    }
    // 所有任务都能按照某种顺序执行完成，无环
    if (uint64_t(map_finish.size()) == tasks_.size()) {
      is_circle_ = false;
      return;
    }
    // 在某次循环发现每个未执行任务都有前置依赖了，存在环依赖，退出
    if (!found) {
      is_circle_ = true;
      return;
    }
  }
}

// 从json字符串中构建tasks
void Graph::BuildFromJson(const string& json_path) {
  Jobs jobs;
  kcfg::ParseFromJsonFile(json_path, jobs);
  unordered_map<string, TaskPtr> task_map;
  // 遍历一遍，拿到task列表
  for (const auto& each : jobs.tasks) {
    TaskPtr A = std::make_shared<Task>(each.task_name);
    task_map.emplace(each.task_name, A);
    tasks_.emplace_back(A);
  }
  // 再遍历一遍，补充依赖关系
  for (const auto& each : jobs.tasks) {
    for (const auto& dep : each.dependencies) {
      if (auto iter = task_map.find(dep); iter != task_map.end()) {
        task_map[each.task_name]->AddDependecy(iter->second);
      }
    }
  }
}

void TaskManager::Clear() {
  dependency_map_.clear();
  map_in_progress_.clear();
  delete input_context_;
  // finish_task_ = 0;
  graph_.reset();
}

}  // namespace taskflow
