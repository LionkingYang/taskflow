#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/test/recmd_test/recmd_op.h"
#include "taskflow/test/recmd_test/struct_define.h"

// 使用json构建图
void run_with_json() {
  // 注册算子
  std::unordered_map<std::string, TaskFunc *> func_map;
  RegisterFuncs(ParseRequest, UU, BlackList, RecallCB, RecallEMB, RecallMerge,
                Rank, Policy, FillResponse);

  // json格式的算子组织方式
  std::string s =
      "{\"tasks\":[{\"task_name\":\"ParseRequest\",\"dependencies\":[]},{"
      "\"task_"
      "name\":\"UU\",\"dependencies\":[\"ParseRequest\"]},{\"task_name\":"
      "\"BlackList\",\"dependencies\":[\"ParseRequest\"]},{\"task_name\":"
      "\"RecallCB\",\"dependencies\":[\"UU\",\"BlackList\"]},{\"task_name\":"
      "\"RecallEMB\",\"dependencies\":[\"UU\",\"BlackList\"]},{\"task_name\":"
      "\"RecallMerge\",\"dependencies\":[\"RecallCB\",\"RecallEMB\"]},{\"task_"
      "name\":\"Rank\",\"dependencies\":[\"RecallMerge\"]},{\"task_name\":"
      "\"Policy\",\"dependencies\":[\"RecallMerge\"]},{\"task_name\":"
      "\"FillResponse\",\"dependencies\":[\"\"]}]}";

  // 初始化总的输入和输出
  RecmdRequest request;
  request.personid = "99999";
  RecmdResponse response;
  auto input = std::any(request);
  auto output = std::any(response);

  // manager进行图运算，从json获取图组织方式
  TaskManager manager(s, &func_map, input, &output);
  if (!manager.Init()) {
    std::cout << "Has circle dependence, check again!" << std::endl;
  } else {
    manager.Run();
  }
  manager.Clear();

  // 打印最终的输出结果
  response = std::any_cast<RecmdResponse>(output);
  for (const auto &each : response.feeds_list) {
    std::cout << each.feedid << ":" << each.posterid << ":"
              << each.score_map.at("aa") << std::endl;
  }
}

int main(int argc, char **argv) {
  run_with_json();
  return 0;
}
