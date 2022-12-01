
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/test/math_test/math_op.h"

int main(int argc, char** argv) {
  // 注册算子
  std::unordered_map<std::string, TaskFunc*> func_map;
  RegisterFuncs(a, b, c, d, e, f);

  // json格式的算子组织方式
  std::string s =
      "{\"tasks\":[{\"task_name\":\"a\",\"dependencies\":[]},{\"task_name\":"
      "\"b\",\"dependencies\":[\"a\"]},{\"task_name\":\"c\",\"dependencies\":["
      "\"a\"]},{\"task_name\":\"d\",\"dependencies\":[\"b\",\"c\"]},{\"task_"
      "name\":\"e\",\"dependencies\":[\"d\"]},{\"task_name\":\"f\","
      "\"dependencies\":[\"e\"]}]}";

  // 初始化总的输入和输出
  auto input = std::any(0);
  auto output = std::any(0);

  // manager进行图运算，从json获取图组织方式
  TaskManager manager(s, &func_map, input, &output);
  if (!manager.Init()) {
    std::cout << "Has circle dependence, check again!" << std::endl;
  } else {
    manager.Run();
  }
  manager.Clear();

  // 打印最终的输出结果
  std::cout << "last res:" << std::any_cast<int>(output) << std::endl;

  return 0;
}
