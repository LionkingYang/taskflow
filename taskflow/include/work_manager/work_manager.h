// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "oneapi/tbb/task_group.h"

using std::vector;
using AnyFunc = std::function<void(void)>;
namespace taskflow {
class WorkManager {
 public:
  WorkManager() {}
  ~WorkManager() { tg_.wait(); }

  void Execute(AnyFunc task) {
    // 随机选取worker执行task
    tg_.run(task);
  }

 private:
  tbb::task_group tg_;
};
}  // namespace taskflow
