import os
import sys
import json


def parse_json_tasks(path: str) -> map:
    content = ""
    with open(path, 'r') as f:
        content = f.read()
    tasks = {}
    try:
        tasks = json.loads(content)
    except:
        raise Exception("wrong format of json, check again")
    return tasks


if __name__ == "__main__":
    main_path = ""
    js_path = ""
    if len(sys.argv) >= 3:
        main_path = sys.argv[1]
        js_path = sys.argv[2]
    else:
        raise Exception("在命令行main文件地址和json地址")

    if not os.path.exists(js_path):
        raise Exception("json文件不存在!")

    tasks = parse_json_tasks(js_path)
    ops = []
    for task in tasks["tasks"]:
        ops.append(task["task_name"])
    ops_str = ",".join(ops)

    template = """  
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/kcfg/kcfg.h"
#include "taskflow/include/macros/macros.h"
#include "taskflow/include/taskflow.h"
#include "$your_project_path/project_op.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {{
  // 注册算子
  std::unordered_map<std::string, TaskFunc *> func_map;
  RegisterFuncs({});

  // json文件地址
  std::string json_path =
      "{}";

  // 初始化总的输入和输出
  auto input = std::any($Your input init value);
  auto output = std::any($Your output init value);
  // 图构建
  std::shared_ptr<Graph> graph = std::make_shared<Graph>(json_path, &func_map);
  // 检测是否成环
  if (graph->CircleCheck()) {{
    std::cout << "Has circle dependence, check again!" << std::endl;
  }} else {{
    // manager进行图运算，从json获取图组织方式
    TaskManager manager(graph, input, &output);
    manager.Run();
  }}
  // fetch your output here
  // std::cout << "last res:" << std::any_cast<$output_type>(output) << std::endl;
}}

int main(int argc, char **argv) {{
  RunGraph();
  return 0;
}}
"""
main_file = template.format(ops_str, js_path)
with open(main_path, 'w') as f:
    f.write(main_file)
