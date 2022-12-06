// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <any>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/kcfg/kcfg.h"
using std::string;
using std::unordered_map;
using std::vector;

struct Job {
  string task_name;
  vector<string> dependencies;
  KCFG_DEFINE_FIELDS(task_name, dependencies)
};

struct Jobs {
  vector<Job> tasks;
  KCFG_DEFINE_FIELDS(tasks)
  bool Init(const std::string& path) {
    kcfg::ParseFromJsonFile(path, *this);
    return true;
  }
};

namespace taskflow {
struct TaskContext {
  const std::any global_input;
  std::any* global_output;
  taskflow::ConcurrentMap<string, std::any> task_output;
  TaskContext(const std::any& input, std::any* output)
      : global_input(input), global_output(output) {}
  ~TaskContext() { Clear(); }
  void Clear() { task_output.clear(); }
};
using TaskFunc = void (*)(taskflow::TaskContext&);
}  // namespace taskflow
