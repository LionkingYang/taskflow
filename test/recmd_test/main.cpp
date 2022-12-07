#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/container/pool.h"
#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/logger/logger.h"
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
  // 图配置和算子路径
  std::string json_path = "/home/lion/taskflow/test/recmd_test/data/test_json";
  std::string script_path = "/home/lion/taskflow/test/recmd_test/ops";

  // 注册图和算子
  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  taskflow::SoScript so_script(script_path);

  // 初始化总的输入和输出
  GET_POOL_OBJ(RecmdRequest, request);
  request.personid = "99999";
  RecmdResponse response;
  std::any input;
  std::any output;

  input = std::any(request);
  output = std::any(response);

  // manager进行图运算，从json获取图组织方式
  for (int i = 0; i < 1000; i++) {
    std::shared_ptr<Graph> graph =
        std::make_shared<Graph>(reloadable_graph.Get());
    if (graph->CircleCheck()) {
      TASKFLOW_CRITICAL("circle reference in graph");
      break;
    }
    taskflow::TaskManager manager(graph, &so_script, input, &output);
    {
      taskflow::LatencyGuard monitor("run task");
      manager.Run();
    }
    // 打印最终的输出结果

    response = std::any_cast<RecmdResponse>(output);
    for (const auto &each : response.feeds_list) {
      TASKFLOW_INFO("{}:{}:{}", each.feedid, each.posterid,
                    each.score_map.at("aa"));
    }
  }
}

int main(int argc, char **argv) {
  RunGraph();
  return 0;
}
