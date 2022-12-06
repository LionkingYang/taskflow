#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/container/pool.h"
#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/reloadable/reloadable_object.h"
#include "taskflow/include/so_handler/so_handler.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/include/utils/latency_guard.h"
#include "test/recmd_test/ops/struct_define.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {
  // 注册算子
  std::string json_path = "/home/lion/taskflow/test/recmd_test/data/test_json";
  std::string script_path = "/home/lion/taskflow/test/recmd_test/ops";

  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  // manager进行图运算，从json获取图组织方式

  // 初始化总的输入和输出
  RecmdRequest request;
  request.personid = "99999";
  RecmdResponse response;
  // for (int i = 0; i < 10; i++) {
  std::any input;
  std::any output;

  input = std::any(request);
  output = std::any(response);

  std::shared_ptr<Graph> graph;
  // manager进行图运算，从json获取图组织方式

  graph = std::make_shared<Graph>(reloadable_graph.Get());

  taskflow::SoScript so_script;
  so_script.Init(script_path);

  // 检测是否成环

  // manager进行图运算，从json获取图组织方式
  // GET_POOL_OBJ(taskflow::TaskManager, manager);
  for (int i = 0; i < 10000; i++) {
    taskflow::TaskManager manager;

    manager.Init(graph, &so_script, input, &output, 1);
    {
      taskflow::LatencyGuard gg("gg");
      manager.Run();
    }
    // 打印最终的输出结果

    response = std::any_cast<RecmdResponse>(output);
    for (const auto &each : response.feeds_list) {
      std::cout << each.feedid << ":" << each.posterid << ":"
                << each.score_map.at("aa") << std::endl;
    }
  }
  // }
}

int main(int argc, char **argv) {
  RunGraph();
  return 0;
}
