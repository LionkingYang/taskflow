// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include "taskflow/include/taskflow.h"

using std::string;
using std::unordered_map;
using std::vector;

namespace taskflow {
void Graph::BuildDependencyMap() {
  for (auto task : tasks_) {
    // 构建依赖map，即某个task如a依赖了哪些task
    // 当某个task的值为0时，说明task没有前置依赖，可以被执行了
    predecessor_count_[task->GetTaskName()] = task->GetPredecessorCount();
    // 构建被依赖map，即某个task被哪些任务依赖
    // 这个是为了方便某个任务执行完成之后，通知相应任务减少自己的前置依赖数
    for (auto dependency : task->GetPredecessors()) {
      if (successor_map_.find(dependency->GetTaskName())) {
        successor_map_[dependency->GetTaskName()].emplace_back(task);
      } else {
        vector<TaskPtr> vec;
        vec.emplace_back(task);
        successor_map_.emplace(dependency->GetTaskName(), vec);
      }
    }
  }
}

void Graph::CircleCheck() {
  // 为了不影响后续正常执行，复制出一份依赖关系出来
  auto successor_map = successor_map_;
  auto map_finish = map_finish_;
  auto tmp_predecessor_count = predecessor_count_;
  // 此处逻辑与执行类似，只是多了一层判断
  // 会判断依赖关系能否完全解耦，即所有任务都可以最终到达没有前置依赖的地步
  while (true) {
    bool found = false;
    for (const auto& task : tasks_) {
      if (tmp_predecessor_count[task->GetTaskName()] == 0 &&
          !map_finish.find(task->GetTaskName())) {
        for (auto each : successor_map[task->GetTaskName()]) {
          tmp_predecessor_count[each->GetTaskName()]--;
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
bool Graph::BuildFromJson(const string& json_path) {
  Jobs jobs;
  if (!kcfg::ParseFromJsonFile(json_path, jobs)) {
    TASKFLOW_CRITICAL("error in parse json");
    return false;
  }
  unordered_map<string, TaskPtr> task_map;
  // 遍历一遍，拿到task列表
  for (const auto& each : jobs.tasks) {
    TaskPtr A =
        std::make_shared<Task>(each.task_name, each.op_name, each.config);
    task_map.emplace(each.task_name, A);
    tasks_.emplace_back(A);
  }
  // 再遍历一遍，补充依赖关系
  for (const auto& each : jobs.tasks) {
    for (const auto& dep : each.dependencies) {
      if (auto iter = task_map.find(dep); iter != task_map.end()) {
        task_map[each.task_name]->AddPredecessor(iter->second);
      }
    }
  }
  return true;
}

std::string Graph::ToString() {
  std::string s;
  for (auto begin = predecessor_count_.begin();
       begin != predecessor_count_.end(); begin++) {
    taskflow::StrAppend(&s, begin->first, ":", begin->second, "\n");
  }
  for (auto begin = successor_map_.begin(); begin != successor_map_.end();
       begin++) {
    taskflow::StrAppend(&s, begin->first, ":");
    for (const auto& each : begin->second) {
      taskflow::StrAppend(&s, each->GetTaskName(), ",");
    }
    taskflow::StrAppend(&s, "\n");
  }
  return s;
}
}  // namespace taskflow
