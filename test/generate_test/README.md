# generate_demo
## 简介
此处为基于data目录下的test_json文件自动生成的项目，包含:
- 主文件: project_main.cpp
- 算子文件: ops/project_op.cpp
## 算子编写

```c++
BeginTask(a) {
  LoadTaskConfig(a, conf);  // 此处加载配置，后续取用可以直接从conf里面取
  GetGlobalInput(int, input_name);  // 此处获取全局输入保存到input_name变量里，这里变量名可以随意更改
  // write your code here，这里写你的业务逻辑
  // a_output必须初始化
  // int a_output = 2;
  // 也可以定义成其他名字，反正最后注入WriteTooutput中就可以了
  WriteToOutput(a, int, a_output);
}
EndTask;

BeginTask(b) {
  LoadTaskConfig(b, conf);
  ReadTaskOutput(a, int, a_output); // 获取a算子的结果，保存到a_output变量中，这里变量名可以随意更改
  // write your code here
  WriteToOutput(b, int, b_output);
}
EndTask;
```


