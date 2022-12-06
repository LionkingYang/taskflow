
#include <dlfcn.h>

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
#include "taskflow/include/reloadable/reloadable_object.h"
#include "taskflow/include/so_handler/so_handler.h"
#include "taskflow/include/taskflow.h"
#include "taskflow/include/utils/latency_guard.h"
#include "test/math_test/math_op.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {
  auto console = spdlog::stdout_color_mt("console");
  // 注册算子
  // std::unordered_map<std::string, TaskFunc> func_map;
  // // RegisterFuncs(b, c, d, e, f);
  // auto so_handler_ =
  //     dlopen("/home/lion/taskflow/test/script/libmath_op.so", RTLD_NOW);
  // if (nullptr == so_handler_) {
  //   console->error(dlerror());
  // }
  // // json文件地址
  std::string json_path = "/home/lion/taskflow/test/math_test/test_json";
  // taskflow::ReloadableObj<Jobs> jsons(json_path);
  // auto content = jsons.Get();

  // for (auto job : content.tasks) {
  //   void* sym = dlsym(so_handler_, ("func_" + job.task_name).c_str());
  //   if (auto error = dlerror(); error != NULL) {
  //     fprintf(stderr, "%s\n", error);
  //   }
  //   auto func = reinterpret_cast<taskflow::TaskFunc>(sym);
  //   if (!func) {
  //     console->error("func {} not exist", job.task_name);
  //   }
  //   func_map.emplace(job.task_name, func);
  // }
  std::string script_path = "/home/lion/taskflow/test/script/";
  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  // taskflow::ReloadableObj<taskflow::SoScript> reloadable_script(script_path);

  // for (int i = 0; i < 10; i++) {
  // 初始化总的输入和输出
  auto input = std::any(0);
  auto output = std::any(0);
  // 图构建
  std::shared_ptr<Graph> graph =
      std::make_shared<Graph>(reloadable_graph.Get());
  taskflow::SoScript so_script;
  so_script.Init(script_path);
  for (int i = 0; i < 1000; i++) {
    // 检测是否成环

    // manager进行图运算，从json获取图组织方式
    // GET_POOL_OBJ(taskflow::TaskManager, manager);
    taskflow::TaskManager manager;
    manager.Init(graph, &so_script, input, &output, 6);
    taskflow::LatencyGuard guard("guard");
    manager.Run();
    // 打印最终的输出结果
    console->info("last res:{}", std::any_cast<int>(output));
  }
  // std::this_thread::sleep_for(2000ms);
  // }
  // {
  //   taskflow::LatencyGuard("dlclose");
  //   dlclose(so_handler_);
  // }
}

int main(int argc, char** argv) {
  RunGraph();

  return 0;
}
