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

BeginFunc(b) {
  int res = ReadTaskOutput(a, int) + 1;
  WriteToOutput(b, res, int);
}

BeginFunc(c) {
  int res = ReadTaskOutput(a, int) + 1;
  WriteToOutput(c, res, int);
}

BeginFunc(d) {
  int res = ReadTaskOutput(b, int) + ReadTaskOutput(c, int);
  WriteToOutput(d, res, int);
}

BeginFunc(e) {
  int res = ReadTaskOutput(d, int) + 1;
  WriteToOutput(e, res, int);
}

BeginFunc(f) {
  int res = ReadTaskOutput(e, int) + 1;
  WriteToFinalOutput(res, int);
}
