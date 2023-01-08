# generate_demo
## 简介
此处为基于data目录下的test_json文件自动生成的项目，包含:
- 主文件: project_main.cpp
- 算子文件: ops/project_op.cpp
## 算子编写

```c++
BEGIN_OP(fetch_input) {
  GET_GLOBAL_INPUT(int, input_name); // 获取全局输入，使用这个宏
  // write your code here
  RETURN_VAL(input_name); // 返回值return
}
END_OP

BEGIN_OP(add_num) {
  GET_INPUT(0, int, a_output);  // 获取第一个输入参数，即任务上游第一个依赖的返回值
  // write your code here
  GET_CONFIG_KEY("num", int, value, 0); // 获取config中num的值，转换为int，默认为0
  int res = a_output + value;
  RETURN_VAL(res);
}
END_OP

BEGIN_OP(accum_mult) {
  GET_INPUT_TO_VEC(int, input_list) // 将上游所有依赖的输出放进input_list中， 注意这里上游输出的数据类型应该一致
  // write your code here
  int res = 1;
  for (const auto& each : input_list) {
    res *= each;
  }
  RETURN_VAL(res);
}
END_OP
```


