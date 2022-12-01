#include "taskflow/include/taskflow.h"

bool TaskManager::Init() {
  BuildDependencyMap();
  if (CircleCheck()) return false;
  while (workers_.size() < worker_nums_) {
    workers_.push_back(std::make_shared<taskflow::TaskWorker>());
  }
  return true;
}

void TaskManager::Run() {
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
    if (uint64_t(map_finish_.size()) == tasks_.size()) {
      std::cout << "All tasks have finished!\n";
      break;
    }
  }
}

void TaskManager::BuildDependencyMap() {
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

bool TaskManager::CircleCheck() {
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
    if (uint64_t(map_finish.size()) == tasks_.size()) {
      return false;
    }
    if (!found) {
      return true;
    }
  }
}
