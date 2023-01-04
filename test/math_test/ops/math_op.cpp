
// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include <any>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "taskflow/include/logger/logger.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/taskflow.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;
using namespace std::chrono_literals;
extern "C" {

BEGIN_OP(add_one) {
  GET_INPUT(0, int, input0);
  int output = input0 + 1;
  std::this_thread::sleep_for(50ms);
  return std::any(output);
}

BEGIN_OP(mult) {
  GET_INPUT(0, int, input0);
  GET_INPUT(1, int, input1);
  LoadTaskConfig(task_name, config);
  int output = input0 * input1;
  std::this_thread::sleep_for(50ms);
  return std::any(output);
}

BEGIN_OP(add) {
  GET_INPUT(0, int, input0);
  GET_INPUT(1, int, input1);
  LoadTaskConfig(task_name, config);
  int output = input0 + input1;
  std::this_thread::sleep_for(50ms);
  return std::any(output);
}

BEGIN_OP(fetch_input) {
  GetGlobalInput(int, global_input);
  std::this_thread::sleep_for(50ms);
  return std::any(global_input);
}

BEGIN_OP(write_output) {
  GET_INPUT(0, int, input0);
  WriteToFinalOutput(int, input0);
  std::this_thread::sleep_for(50ms);
  return std::any(0);
}
}
