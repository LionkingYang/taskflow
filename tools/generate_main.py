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
    op_path = ""
    js_path = ""
    main_path = ""
    if len(sys.argv) >= 4:
        op_path = sys.argv[1]
        js_path = sys.argv[2]
        main_path = sys.argv[3]
    else:
        raise Exception("在命令行算子地址和json地址以及生成的目的文件名")

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

#include "taskflow/include/container/pool.h"
#include "taskflow/include/kcfg/kcfg.h"
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
void RunGraph() {{
  // json文件地址
  std::string json_path = "{}";
  // 算子目录
  std::string script_path = "{}";
  // 热加载图配置
  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  // 热加载算子图构建
  taskflow::SoScript so_script(script_path);
  // 初始化总的输入和输出
  auto input = std::any(0);
  auto output = std::any(0);
  // 从热更新图里获取最新的图
  std::shared_ptr<Graph> graph =
      std::make_shared<Graph>(reloadable_graph.Get());
  if (graph->GetCircle()) {{
    TASKFLOW_CRITICAL("graph has circle reference, check agin");
  }} else {{
    // manager进行图运算，从json获取图组织方式
    taskflow::TaskManager manager(graph, &so_script, input, &output);
    manager.Run();
    // 打印最终的输出结果
    // TASKFLOW_INFO("last res:{{}}", std::any_cast<int>(output));
  }}
}}

int main(int argc, char** argv) {{
  RunGraph();
  return 0;
}}

"""
main_file = template.format(js_path, op_path)
with open(main_path, 'w') as f:
    f.write(main_file)
