
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
  int res_a = res + 1;
  WriteToOutput(a, int, res_a);
}
EndTask;

BeginTask(b) {
  ReadTaskOutput(a, int, res_a);
  int res_b = res_a + 1;
  WriteToOutput(b, int, res_b);
}
EndTask;

BeginTask(c) {
  ReadTaskOutput(a, int, res_a);
  int res_c = res_a + 1;
  WriteToOutput(c, int, res_c);
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
  ReadTaskOutput(d, int, res_d);
  int res_e = res_d + 1;
  WriteToOutput(e, int, res_e);
}
EndTask;

BeginTask(f) {
  ReadTaskOutput(e, int, res_e);
  int res_f = res_e + 1;
  WriteToFinalOutput(int, res_f);
}
EndTask;
}