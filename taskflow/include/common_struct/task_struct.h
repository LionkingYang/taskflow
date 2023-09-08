// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <any>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/json/json_parser.h"
using std::string;
using std::unordered_map;
using std::vector;

struct NodeConf {
  string task_name;
  vector<string> dependencies;
  string op_name;
  string config;
  string condition;
  bool async = false;
  int timeout = 100;
  KCFG_DEFINE_FIELDS(task_name, op_name, config, condition, async, dependencies,
                     timeout)
};

struct GraphConf {
  vector<NodeConf> tasks;
  int timeout = 500;
  KCFG_DEFINE_FIELDS(tasks, timeout)
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
  taskflow::ConcurrentMap<string, std::string> task_env;
  std::unordered_map<string, std::unordered_map<string, string>> task_config;
  TaskContext(const std::any& input, std::any* output)
      : global_input(input), global_output(output) {}
  ~TaskContext() { Clear(); }
  void Clear() { task_output.clear(); }
};
using TaskFunc = std::any (*)(taskflow::TaskContext&,
                              const std::vector<std::string>&,
                              const string& task_name);
}  // namespace taskflow
