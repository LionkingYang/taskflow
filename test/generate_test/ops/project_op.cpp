
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/logger/logger.h"
#include "taskflow/include/taskflow.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

extern "C" {

BEGIN_OP(fetch_input) {
  GET_GLOBAL_INPUT(int, input_name);
  // write your code here
  RETURN_VAL(input_name);
}
END_OP;

BEGIN_OP(add_num) {
  GET_INPUT(0, int, a_output);
  // write your code here
  GET_CONFIG_KEY("num", int, value, 0);
  int res = a_output + value;
  RETURN_VAL(res);
}
END_OP;

BEGIN_OP(mult_num) {
  GET_INPUT(0, int, a_output);
  // write your code here
  GET_CONFIG_KEY("num", int, value, 0);
  int res = a_output * value;
  RETURN_VAL(res);
}
END_OP;

BEGIN_OP(accum_mult) {
  GET_INPUT_TO_VEC(int, input_list)
  // write your code here
  int res = 1;
  for (const auto& each : input_list) {
    res *= each;
  }
  RETURN_VAL(res);
}
END_OP;

BEGIN_OP(write_output) {
  GET_INPUT(0, int, d_output);
  // write your code here
  WRITE_TO_FINAL_OUTPUT(int, d_output);
  RETURN_VAL(0);
}
END_OP;
}
