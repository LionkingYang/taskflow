#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "taskflow/include/taskflow.h"

int main(int argc, char **argv) {
  int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;

  AnyFunc func_a = [&]() mutable { a++; };
  AnyFunc func_b = [&]() mutable { b += a; };
  AnyFunc func_c = [&]() mutable { c += a; };
  AnyFunc func_d = [&]() mutable { d = b + c; };
  AnyFunc func_e = [&]() mutable { e = d + 1; };
  AnyFunc func_f = [&]() mutable { f = e + 1; };
  // 初始化task
  Task A("task A", &func_a);
  Task B("task B", &func_b);
  Task C("task C", &func_c);
  Task D("task D", &func_d);
  Task E("task E", &func_e);
  Task F("task F", &func_f);
  // 构建依赖关系
  B.AddDependecy(&A);
  C.AddDependecy(&A);
  D.AddDependecy(&B);
  D.AddDependecy(&C);
  E.AddDependecy(&D);
  F.AddDependecy(&E);
  // 写入task列表
  std::vector<Task *> tasks;
  tasks.emplace_back(&A);
  tasks.emplace_back(&B);
  tasks.emplace_back(&C);
  tasks.emplace_back(&D);
  tasks.emplace_back(&E);
  tasks.emplace_back(&F);

  // manager进行图运算
  TaskManager manager(tasks);
  if (!manager.Init()) {
    std::cout << "Has circle dependence, check again!" << std::endl;
  } else {
    manager.Run();
  }
  std::cout << "last res:" << f << std::endl;
  return 0;
}
