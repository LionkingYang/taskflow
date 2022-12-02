#pragma once
#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/taskflow.h"

// 算子构建
// BeginFunc(a) 此处a与json里的task_name要保持一致
// 写入算子逻辑
// WriteTaskOutput(task_name) 向相应的算子中写入结果
// ReadTaskOutput(task_name)
// 读取相应算子的结果，注意这里一定要和依赖关系保持一致，不然会core

BeginFunc(a) {
  int res = Input(int) + 1;
  WriteToOutput(a, res, int);
}
EndFunc

BeginFunc(b) {
  ReadTaskOutput(a, int, res);
  res += 1;
  WriteToOutput(b, res, int);
}
EndFunc

BeginFunc(c) {
  ReadTaskOutput(a, int, res);
  res += 1;
  WriteToOutput(c, res, int);
}
EndFunc

BeginFunc(d) {
  ReadTaskOutput(b, int, res_b);
  ReadTaskOutput(c, int, res_c);
  int res = res_b + res_c;
  WriteToOutput(d, res, int);
}
EndFunc

BeginFunc(e) {
  ReadTaskOutput(d, int, res);
  res += 1;
  WriteToOutput(e, res, int);
}
EndFunc

BeginFunc(f) {
  ReadTaskOutput(e, int, res);
  res += 1;
  WriteToFinalOutput(res, int);
}
EndFunc
