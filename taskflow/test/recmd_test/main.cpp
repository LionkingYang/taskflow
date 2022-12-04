#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/container/pool.h"
#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/test/recmd_test/recmd_op.h"
#include "taskflow/test/recmd_test/struct_define.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {
  // 注册算子
  std::unordered_map<std::string, TaskFunc *> func_map;
  RegisterFuncs(ParseRequest, UU, BlackList, RecallCB, RecallEMB, RecallMerge,
                Rank, Policy, FillResponse);

  // json文件地址
  std::string json_path =
      "/home/lion/MyTaskFlow/taskflow/test/recmd_test/test_json";

  // manager进行图运算，从json获取图组织方式
  std::shared_ptr<Graph> graph = std::make_shared<Graph>(json_path, &func_map);
  if (graph->CircleCheck()) {
    std::cout << "Has circle dependence, check again!" << std::endl;
  } else {
    // 初始化总的输入和输出
    RecmdRequest request;
    request.personid = "99999";
    RecmdResponse response;
    auto input = std::any(request);
    auto output = std::any(response);
    // manager进行图运算，从json获取图组织方式
    GET_POOL_OBJ(taskflow::TaskManager, manager);
    manager.Init(graph, input, &output);
    manager.Run();
    // 打印最终的输出结果
    response = std::any_cast<RecmdResponse>(output);
    for (const auto &each : response.feeds_list) {
      std::cout << each.feedid << ":" << each.posterid << ":"
                << each.score_map.at("aa") << std::endl;
    }
  }
}

int main(int argc, char **argv) {
  RunGraph();
  return 0;
}
