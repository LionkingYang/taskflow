
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

BeginFunc(a) {
  GetGlobalInput(int, input_name);
  // write your code here
  int a_output = input_name + 1;
  WriteToOutput(a, int, a_output);
}
EndFunc;

BeginFunc(b) {
  ReadTaskOutput(a, int, a_output);
  int b_output = a_output + 1;
  // write your code here
  WriteToOutput(b, int, b_output);
}
EndFunc;

BeginFunc(c) {
  ReadTaskOutput(a, int, a_output);
  int c_output = a_output + 1;
  // write your code here
  WriteToOutput(c, int, c_output);
}
EndFunc;

BeginFunc(d) {
  ReadTaskOutput(b, int, b_output);
  ReadTaskOutput(c, int, c_output);
  // write your code here
  int d_output = b_output + c_output;
  WriteToOutput(d, int, d_output);
}
EndFunc;

BeginFunc(e) {
  ReadTaskOutput(d, int, d_output);
  // write your code here
  int final_output = d_output + 1;
  WriteToFinalOutput(int, final_output);
}
EndFunc;
}
