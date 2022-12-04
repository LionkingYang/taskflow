// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.
#pragma once
#include <memory>
#include <vector>

#include "taskflow/include/async_task/async_task.h"

using std::vector;
using WorkerPtr = std::shared_ptr<taskflow::AsyncWorker>;

namespace taskflow {
class WorkManager {
 public:
  explicit WorkManager(uint64_t worker_nums) {
    while (workers_.size() < worker_nums) {
      workers_.push_back(std::make_shared<taskflow::AsyncWorker>());
    }
  }

  void Execute(AnyFunc task) {
    // 随机选取worker执行task
    uint32_t cursor = random() % workers_.size();
    auto worker = workers_[cursor];
    worker->Post(task, false);
  }

 private:
  vector<WorkerPtr> workers_;
};
}  // namespace taskflow
