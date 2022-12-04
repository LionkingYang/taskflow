
// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/taskflow.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

BeginFunc(a) {
  GetGlobalInput(int, res);
  res += 1;
  WriteToOutput(a, int, res);
}
EndFunc;

BeginFunc(b) {
  ReadTaskOutput(a, int, res);
  res += 1;
  WriteToOutput(b, int, res);
}
EndFunc;

BeginFunc(c) {
  ReadTaskOutput(a, int, res);
  res += 1;
  WriteToOutput(c, int, res);
}
EndFunc;

BeginFunc(d) {
  ReadTaskOutput(b, int, res_b);
  ReadTaskOutput(c, int, res_c);
  int res_d = res_b + res_c;
  WriteToOutput(d, int, res_d);
}
EndFunc;

BeginFunc(e) {
  ReadTaskOutput(d, int, res);
  res += 1;
  WriteToOutput(e, int, res);
}
EndFunc;

BeginFunc(f) {
  ReadTaskOutput(e, int, res);
  res += 1;
  WriteToFinalOutput(int, res);
}
EndFunc;