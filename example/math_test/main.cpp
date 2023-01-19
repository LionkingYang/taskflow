#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/container/pool.h"
#include "taskflow/include/json/json_parser.h"
#include "taskflow/include/logger/logger.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/reloadable/reloadable_object.h"
#include "taskflow/include/so_handler/so_handler.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/include/utils/latency_guard.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {
  // json文件地址
  std::string json_path =
      "/data/svn2/taskflow/example/math_test/data/test_json";
  // 算子目录
  std::string script_path = "/data/svn2/taskflow/example/math_test/ops";
  // 热加载图配置
  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  // 热加载算子图构建
  taskflow::SoScript so_script(script_path);
  auto task = [&]() {
    // 初始化总的输入和输出
    auto output = std::any(0);
    for (int i = 0; i < 1000; i++) {
      auto input = std::any(static_cast<int>(random() % 1000));
      TASKFLOW_INFO("{} input is:{}", i, std::any_cast<int>(input));
      // 从热更新图里获取最新的图
      std::shared_ptr<Graph> graph =
          std::make_shared<Graph>(reloadable_graph.Get());
      if (graph->GetCircle()) {
        TASKFLOW_CRITICAL("graph has circle reference, check agin");
        break;
      } else {
        // manager进行图运算，从json获取图组织方式
        taskflow::LatencyGuard guard("run graph");
        taskflow::TaskManager manager(graph, &so_script, input, &output);
        manager.SetEnv("test", "100");
        manager.Run();
        // 打印最终的输出结果
        TASKFLOW_INFO("last res:{}", std::any_cast<int>(output));
      }
    }
  };
  std::vector<std::thread> threads;
  for (int i = 0; i < 10; i++) threads.push_back(std::thread(task));
  for (auto& thread : threads) thread.join();
}

void RunGraphWithNum(int num) {
  // json文件地址
  std::string json_path =
      "/data/svn2/taskflow/example/math_test/data/test_json";
  // 算子目录
  std::string script_path = "/data/svn2/taskflow/example/math_test/ops";
  // 热加载图配置
  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  // 热加载算子图构建
  taskflow::SoScript so_script(script_path);
  // 初始化总的输入和输出
  auto output = std::any(0);
  auto input = std::any(num);

  // TASKFLOW_INFO("input is:{}", std::any_cast<int>(input));
  // 从热更新图里获取最新的图
  std::shared_ptr<Graph> graph =
      std::make_shared<Graph>(reloadable_graph.Get());
  if (graph->GetCircle()) {
    TASKFLOW_CRITICAL("graph has circle reference, check agin");
    return;
  } else {
    // manager进行图运算，从json获取图组织方式
    taskflow::LatencyGuard guard("run graph");
    taskflow::TaskManager manager(graph, &so_script, input, &output);
    manager.SetEnv("test", "100");
    manager.Run();
    // 打印最终的输出结果
    TASKFLOW_INFO("last res:{}", std::any_cast<int>(output));
  }
}

int main(int argc, char** argv) {
  if (argc == 1) {
    RunGraph();
  } else {
    int num = 0;
    if (absl::SimpleAtoi(argv[1], &num)) {
      RunGraphWithNum(num);
    }
  }
  return 0;
}
