
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
  GET_GLOBAL_INPUT($input_type, input_name);
  // write your code here
  RETURN_VAL(your_output);
}
END_OP

BEGIN_OP(add_num) {
  GET_INPUT(0, $param_type, a_output);
  // write your code here
  RETURN_VAL(your_output);
}
END_OP

BEGIN_OP(mult_num) {
  GET_INPUT(0, $param_type, a_output);
  // write your code here
  RETURN_VAL(your_output);
}
END_OP

BEGIN_OP(accum_mult) {
  GET_INPUT(0, $param_type, a_output);
  GET_INPUT(1, $param_type, b_output);
  GET_INPUT(2, $param_type, c_output);
  // write your code here
  RETURN_VAL(your_output);
}
END_OP

BEGIN_OP(accum_add) {
  GET_INPUT(0, $param_type, a_output);
  GET_INPUT(1, $param_type, b_output);
  GET_INPUT(2, $param_type, d_output);
  // write your code here
  RETURN_VAL(your_output);
}
END_OP

BEGIN_OP(write_output) {
  GET_INPUT(0, $param_type, e_output);
  // write your code here
  WRITE_TO_FINAL_OUTPUT($output_type, final_output);
  RETURN_VAL(your_output);
}
END_OP
}
