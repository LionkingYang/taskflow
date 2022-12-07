
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

BeginTask(a) {
  LoadTaskConfig(a, conf);
  GetGlobalInput(int, input_name);
  // write your code here
  WriteToOutput(a, int, a_output);
}
EndTask;

BeginTask(b) {
  LoadTaskConfig(b, conf);
  ReadTaskOutput(a, int, a_output);
  // write your code here
  WriteToOutput(b, int, b_output);
}
EndTask;

BeginTask(c) {
  LoadTaskConfig(c, conf);
  ReadTaskOutput(a, int, a_output);
  // write your code here
  WriteToOutput(c, int, c_output);
}
EndTask;

BeginTask(d) {
  LoadTaskConfig(d, conf);
  ReadTaskOutput(b, int, b_output);
  ReadTaskOutput(c, int, c_output);
  // write your code here
  WriteToOutput(d, int, d_output);
}
EndTask;

BeginTask(e) {
  LoadTaskConfig(e, conf);
  ReadTaskOutput(d, int, d_output);
  // write your code here
  WriteToFinalOutput(int, final_output);
}
EndTask;
}
