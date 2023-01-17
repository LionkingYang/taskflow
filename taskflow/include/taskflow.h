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

#include "taskflow/include/common_struct/task_struct.h"
#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/graph.h"
#include "taskflow/include/logger/logger.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/so_handler/so_handler.h"
#include "taskflow/include/task.h"
#include "taskflow/include/utils/string_utils.h"
#include "taskflow/include/work_manager/work_manager.h"

using std::atomic;
using std::string;
using std::unordered_map;
using std::vector;
using taskflow::Task;
using taskflow::TaskPtr;

namespace taskflow {

class TaskManager {
 public:
  // 使用已经建立好依赖关系的tasks列表进行初始化
  TaskManager(std::shared_ptr<Graph> graph, taskflow::SoScript* so_script,
              const std::any& input, std::any* output);
  void Run();
  ~TaskManager() { Clear(); }
  void Clear();
  void SetEnv(const string& key, const string& value) {
    input_context_->task_env.emplace(key, value);
  }

 private:
  string ToString();
  bool MatchCondition(const string& condition);
  bool OneCondition(const string& expr);

 private:
  taskflow::ConcurrentMap<string, std::shared_ptr<std::atomic_int>>
      atomic_predecessor_count_;
  std::shared_ptr<Graph> graph_;
  taskflow::SoScript* so_script_;
  taskflow::ConcurrentMap<string, int> map_in_progress_;
  TaskContext* input_context_;
  std::atomic<int> finish_num_ = 0;
  taskflow::ConcurrentMap<string, int> switch_map_;
};
}  // namespace taskflow
