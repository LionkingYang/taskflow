
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

BEGIN_OP(AddNum) {
  GET_INPUT(0, int, input0);
  GET_CONFIG_KEY("num", int, num, 0);
  int output = input0 + num;
  RETURN_VAL(output);
}
END_OP

BEGIN_OP(MultNum) {
  GET_INPUT(0, int, input0);
  GET_CONFIG_KEY("num", int, num, 0);
  int output = input0 * num;
  RETURN_VAL(output);
}
END_OP

BEGIN_OP(Mult) {
  GET_INPUT(0, int, input0);
  GET_INPUT(1, int, input1);
  int output = input0 * input1;
  RETURN_VAL(output);
}
END_OP

BEGIN_OP(Add) {
  GET_INPUT(0, int, input0);
  GET_INPUT(1, int, input1);
  int output = input0 + input1;
  RETURN_VAL(output);
}
END_OP

BEGIN_OP(AccumAdd) {
  GET_INPUT_TO_VEC(int, input_list);
  int res = 0;
  for (const auto& each : input_list) {
    res += each;
  }
  RETURN_VAL(res);
}
END_OP

BEGIN_OP(AccumMult) {
  GET_INPUT_TO_VEC(int, input_list);
  int res = 1;
  for (const auto& each : input_list) {
    res *= each;
  }
  RETURN_VAL(res);
}
END_OP

BEGIN_OP(FetchInput) {
  GET_GLOBAL_INPUT(int, global_input);
  RETURN_VAL(global_input);
}
END_OP

BEGIN_OP(WriteOutput) {
  GET_INPUT(0, int, input0);
  WRITE_TO_FINAL_OUTPUT(int, input0);
  RETURN_VAL(0);
}
END_OP
}
