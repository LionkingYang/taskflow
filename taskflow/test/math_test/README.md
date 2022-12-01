# 实现简单的数学计算
可以手动构建依赖 or 从json中提取依赖构建图

本示例依赖关系见test_json

input->a->b/c->d->e->f->output

算子a: input+1

算子b: a+1

算子c: a+1

算子d: b+c

算子e: d+1

算子f: e+1并且输出结果

执行方法：

bazel run //:math_test
