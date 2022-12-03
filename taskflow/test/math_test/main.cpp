
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/test/math_test/math_op.h"

// 不使用json，手动构建图，不推荐
void run_without_json() {
  int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
  TaskFunc func_a = [&](TaskContext *context) { a += Input(int) + 1; };
  TaskFunc func_b = [&](TaskContext *) { b += a + 1; };
  TaskFunc func_c = [&](TaskContext *) { c += a + 1; };
  TaskFunc func_d = [&](TaskContext *) { d = b + c; };
  TaskFunc func_e = [&](TaskContext *) { e = d + 1; };
  TaskFunc func_f = [&](TaskContext *) { f = e + 1; };
  // 初始化task
  auto A = std::make_shared<Task>("task A", &func_a);
  auto B = std::make_shared<Task>("task B", &func_b);
  auto C = std::make_shared<Task>("task C", &func_c);
  auto D = std::make_shared<Task>("task D", &func_d);
  auto E = std::make_shared<Task>("task E", &func_e);
  auto F = std::make_shared<Task>("task F", &func_f);

  // 构建依赖关系
  B->AddDependecy(A);
  C->AddDependecy(A);
  D->AddDependecy(B);
  D->AddDependecy(C);
  E->AddDependecy(D);
  F->AddDependecy(E);
  // 写入task列表
  std::vector<std::shared_ptr<Task>> tasks;
  tasks.emplace_back(A);
  tasks.emplace_back(B);
  tasks.emplace_back(C);
  tasks.emplace_back(D);
  tasks.emplace_back(E);
  tasks.emplace_back(F);

  // manager进行图运算
  // 初始化总的输入和输出
  auto input = std::any(0);
  auto output = std::any(0);

  TaskManager manager(tasks, input, &output);
  if (!manager.Init()) {
    std::cout << "Has circle dependence, check again!" << std::endl;
  } else {
    manager.Run();
  }
  manager.Clear();
  std::cout << "last res:" << f << std::endl;
}

// 使用json构建图
void run_with_json() {
  // 注册算子
  std::unordered_map<std::string, TaskFunc *> func_map;
  RegisterFuncs(a, b, c, d, e, f);

  // json格式的算子组织方式
  std::string s =
      "{\"tasks\":[{\"task_name\":\"a\",\"dependencies\":[]},{\"task_name\":"
      "\"b\",\"dependencies\":[\"a\"]},{\"task_name\":\"c\",\"dependencies\":"
      "["
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
}

int main(int argc, char **argv) {
  run_without_json();
  run_with_json();
  return 0;
}
