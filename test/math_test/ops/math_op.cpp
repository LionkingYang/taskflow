
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
#include "taskflow/include/taskflow.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;
using namespace std::chrono_literals;
extern "C" {

BeginTask(a) {
  GetGlobalInput(int, res);
  DebugConfig(a);
  res += 1;
  WriteToOutput(a, int, res);
}
EndTask;

BeginTask(b) {
  ReadTaskOutput(a, int, res);
  res += 1;
  WriteToOutput(b, int, res);
}
EndTask;

BeginTask(c) {
  ReadTaskOutput(a, int, res);
  res += 1;
  WriteToOutput(c, int, res);
}
EndTask;

BeginTask(d) {
  ReadTaskOutput(b, int, res_b);
  ReadTaskOutput(c, int, res_c);
  int res_d = res_b + res_c;
  WriteToOutput(d, int, res_d);
}
EndTask;

BeginTask(e) {
  ReadTaskOutput(d, int, res);
  res += 1;
  WriteToOutput(e, int, res);
}
EndTask;

BeginTask(f) {
  ReadTaskOutput(e, int, res);
  res += 1;
  WriteToFinalOutput(int, res);
}
EndTask;
}