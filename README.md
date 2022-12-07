# TaskFlow

## 简介

### TaskFlow是什么？

顾名思义，TaskFlow是一堆任务的集合，按照某种特定的数据依赖，组成类似流图的关系，很多大型的任务都可以分拆为各种小型任务组成的TaskFlow。

TaskFlow有两个重要的组成方式，数据流和控制流，数据流表示数据通过输入输出，在TaskFlow的各个任务之间流动；控制流则代表TaskFlow中各个任务的执行顺序。

TaskFlow的执行存在以下难点：

- 首先数据流的依赖关系需要谨慎梳理，否则容易出现数据循环依赖即流图出现成环的现象，这样的TaskFlow是无法完成的，而很多大型的任务，去判断是否存在这样的循环数据依赖非常困难。
- 在实现合理的数据依赖后，在设置控制流时，需要考虑到数据流中的各种依赖，否则，则会导致整个TaskFlow执行的崩溃。

**本项目主要集中与以上两个问题，希望提供一个简单易用并且高性能的TaskFlow执行框架。**

### 本框架支持什么？

针对以上两个问题，本项目提供了以下的解决方案：

- 提供了循环依赖检查工具，方便使用者确认自己的任务流中是否存在循环引用的情况。同时，框架执行时会进行图的校验，如果存在循环引用，会报错提醒框架使用者。
- 使用拓扑排序的思想规划任务的执行路径，同时利用高性能的线程池工具，提高TaskFlow执行的并行度，减小线程切换带来的损失。

同时，本框架还提供这些功能：

- 简单的算子编写体验
- 根据图配置，一键生成算子和项目代码
- 算子和图配置的相互校验
- 算子和图的热更新
- 算子配置注入
- 基于spdlog的多级日志支持
- 简单易用的对象池
- 简单易用的LatencyDebug工具
- 简单易用的json解析接口
- ... ...

## 入门

### 图配置文件

采用json进行图的定义，具体格式如下：

```json
{
	"input_type": "int",
	"output_type": "int",
	"tasks": [{
		"task_name": "",
		"dependencies": [],
		"type": "int",
		"use_input": "1",
		"config": "a=1|b=2",
		"final_output*": "1"
	}]
}
```

- input_type: **可选**。全局输入的数据类型，如果填写了此字段，自动生成项目算子代码时会在**GetGlobalInput**获取全局输入时自动填充此类型。
- output_type: **可选**。全局输出的数据类型，如果填写了此字段，自动生成项目算子代码时会在**WriteToFinalOutput**写入全局输出时自动填充此类型。
- tasks: **必填**。数组，内部填充所有的任务描述。
- task_name: **必填**。任务名，**任务唯一标识**，此任务名应与算子中对应的算子名一致。
- dependencies: **必填，可为空**。任务依赖，数组。此任务依赖的其他任务，内填写其他任务的task_name。
- type: **可选**。任务输出的数据类型。如果填写了此字段，自动生成项目算子代码时会在**ReadTaskOutput**获取该算子输出时自动填充此类型。
- use_input: **可选**。是否使用全局输入。如果填写了此字段且字段值为"1"，自动生成项目算子代码时，该算子中会出现**GetGlobalInput**宏。
- config: **可选**。配置格式：**k1=v1|k2=v2|k3=v3**。任务的配置，注入算子的配置，可在算子中直接用宏**LoadTaskConfig**获取，获取后的配置为**unordered_map**格式。
- final_output: **可选，但是只能有一个算子设置**。全图只能有一个对外输出算子， 如果填写了此字段且字段值为"1"，自动生成项目算子代码时，该算子中会出现**WriteToFinalOutput**宏。

### 算子编写

基于多种宏的支持，算子编写十分简单，具体结构如下（下图为自动生成算子）：

```c++
BeginTask(a) {
  LoadTaskConfig(a, conf);
  GetGlobalInput(int, input_name);
  // write your code here
  WriteToOutput(a, int, a_output);
}
EndTask;
BeginTask(b) {
  LoadTaskConfig(b, conf);
  ReadTaskOutput(a, int, a_output);
  // write your code here
  WriteToOutput(b, int, b_output);
}
EndTask;
BeginTask(e) {
  LoadTaskConfig(e, conf);
  ReadTaskOutput(b, int, d_output);
  // write your code here
  WriteToFinalOutput(int, final_output);
}
EndTask;
```

其中比较重要的宏：

- **BeginTask(task_name) { ... ...} EndTask;** 该宏为一对组合，标记着任务算子的开始与结束，其中BeginTask()括号中填写任务名，**该任务名应与图配置文件中的任务名对应**。

- **LoadTaskConfig(task_name， conf)** 该宏读取对应的算子配置，并且把数据赋值给conf，conf为一个unordered_map。

- **DebugConfig(task_name)** debug宏，遍历算子配置并且打印log。

- **GetGlobalInput(type, input_name)** 获取图的全局输入，并且赋值给type类型的input_name变量。此处需要注意type类型需要与算子的全局输入一致，否则会有bad_cast错误的风险。(**采用自动生成的算子可以规避此风险**)

- **ReadTaskOutput(task_name, type, task_output)** 获取task_name算子的输入，并且赋值给type类型的名为task_output的变量上。此处也需注意type类型需要与算子真实的输出类型一致，否则会有bad_cast的风险，同时也需要保证在算子里只获取图配置里定义的依赖算子的输出，如：a算子依赖b, c算子的数据，那么在a算子中只能读取b,c算子的数据，否则会有bad_cast的风险。(**采用自动生成的算子可以规避此风险**)

- **WriteToOutput(task_name, type, task_output)** 将type类型名为task_output的变量值赋值给任务的输出。此处赋值之后，依赖该任务的其他任务可以通过**ReadTaskOutput** 算子获取到该算子的输出。

- **WriteToFinalOutput(type, final_output)**将type类型名为final_output的变量值赋给全局输出。此处需要注意type需与你定义的全局输出类型一致，否则会有bad_cast的风险。 (**采用自动生成的算子可以规避此风险**)

  **需要注意上面提到的input_name，task_output，final_output均可以随意自定义变量名，并不是必须写死这几个，对于input数据，ReadTaskOutput算子和GetGlobalInput算子都会对变量进行定义，因此直接取用赋值就行；但是对于需要写进output的变量，如task_output，final_output，在业务代码里需要先定义，不然会出现编译问题**

### 执行

推荐的目录结构如图：

```.
├── data
│   └── test_json
├── deps
│   └── struct_define.h
├── main.cpp
└── ops
    └── recmd_op.cpp
```

main文件示例：

```c++
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
#include "test/recmd_test/deps/struct_define.h"

using taskflow::Graph;
using taskflow::TaskContext;
using taskflow::TaskFunc;
using taskflow::TaskManager;

// 使用json构建图
void RunGraph() {
  // 图配置和算子路径
  std::string json_path = "此处填写你的json配置的文件地址";
  std::string script_path = "此处填写你的算子目录";

  // 注册图和算子，都是可热更新的
  taskflow::ReloadableObj<taskflow::Graph> reloadable_graph(json_path);
  // 算子so handler，可热更新
  taskflow::SoScript so_script(script_path);

  // 初始化总的输入和输出
  // 从对象池里Get一个出来
  GET_POOL_OBJ(RecmdRequest, request);
  // 填request数据
  request.personid = "99999";
  // response
  RecmdResponse response;
  // 转化输入输出，方便统一的算子输入输出
  std::any input = std::any(request);
  std::any output = std::any(response);

  // manager进行图运算，从json获取图组织方式
  for (int i = 0; i < 1000; i++) {
    // get一个当前的图出来
    std::shared_ptr<Graph> graph =
        std::make_shared<Graph>(reloadable_graph.Get());
    // 每次reload graph之后，判断是否成环
    if (graph->GetCircle()) {
      TASKFLOW_CRITICAL("circle reference in graph");
      break;
    }
    // 初始化manager
    // 参数分别为graph，算子，input，output
    taskflow::TaskManager manager(graph, &so_script, input, &output);
    {
      // debug一下耗时
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

```



算子编译bazel配置:

```pytho
cc_binary(
    name = "recmd_op",
    srcs = glob([
        "test/recmd_test/ops/*.cpp",
        "test/recmd_test/deps/*.h",
    ]),
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC -shared -rdynamic",
    ],
    linkshared = True,
    linkstatic = True,
    deps = ["//:task_flow_dep"],
)
```

编译命令：

```she
bazel build //:recmd_op
```

编译完成后算子so会在项目目录的bazel-bin目录中：libxxx.so 。**需要将该so文件放到你的算子文件夹下。算子热更新时，算子名需要重命名一下，与线上在使用的算子不能同名，如线上如果是xx.so，可改名为xx2.so。**

binary编译bazel配置：

```pyth
cc_binary(
    name = "recmd_test",
    srcs = glob([
        "test/recmd_test/**/*.cpp",
        "test/recmd_test/**/*.h",
    ]),
    linkopts = [
        "-lpthread",
        "-rdynamic",
        "-fPIC",
        "-ldl",
    ],
    deps = ["//:task_flow_dep"],
)
```

运行命令：

```she
 bazel run //:recmd_test 
```

此时能进行算子运算并且打印出：

```shell
[2022-12-07 14:28:32.612] [info] [latency_guard.h:27] run task cost 0.027583 ms.
[2022-12-07 14:28:32.612] [info] [main.cpp:64] eeeee:55555:0.6
[2022-12-07 14:28:32.612] [info] [main.cpp:64] fffff:66666:0.5
[2022-12-07 14:28:32.612] [info] [main.cpp:64] ccccc:33333:0.2
[2022-12-07 14:28:32.612] [info] [main.cpp:64] ddddd:44444:0.1
```

## 实用工具

主要工具都在tool目录下，包括：

- 图配置检查工具：check_json_file.py
- 算子检查工具：check_ops.py
- 主文件生成工具：generate_main.py
- 算子生成工具：generate_op.py
- 项目生成工具：generate_project.py

### 一键生成项目

```shell
cd tools
python3 generate_project.py project_path(你的项目目录) json_file_path(你的图配置，如graph.json)
```

生成前，只有图配置json文件：

```.
├── data
   └── test_json
```

生成后目录如下：

```.
├── data
│   └── test_json
├── ops
│   └── project_op.cpp
└── project_main.cpp
```

输出算子和主文件，用户可以自行执行命令后去体验。

### 图配置检查
### 算子检查
### 算子生成
### 主文件生成