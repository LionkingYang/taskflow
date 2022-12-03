
#pragma once
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/taskflow.h"

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
  WriteToFinalOutput(int, res);
}
EndFunc;