
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "taskflow/include/container/pool.h"
#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/taskflow.h"
#include "test/math_test/math_op.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {
  auto console = spdlog::stdout_color_mt("console");
  // 注册算子
  std::unordered_map<std::string, TaskFunc *> func_map;
  RegisterFuncs(a, b, c, d, e, f);

  // json文件地址
  std::string json_path =
      "/home/lion/MyTaskFlow/taskflow/test/math_test/test_json";

  // 初始化总的输入和输出
  auto input = std::any(0);
  auto output = std::any(0);
  // 图构建
  std::shared_ptr<Graph> graph = std::make_shared<Graph>(json_path, &func_map);
  // 检测是否成环
  if (graph->CircleCheck()) {
    console->error("Has circle dependence, check again!");
  } else {
    // manager进行图运算，从json获取图组织方式
    GET_POOL_OBJ(taskflow::TaskManager, manager);
    manager.Init(graph, input, &output);
    manager.Run();
    // 打印最终的输出结果
    console->info("last res:{}", std::any_cast<int>(output));
  }
}

int main(int argc, char **argv) {
  RunGraph();
  return 0;
}
