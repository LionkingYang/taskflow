// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "taskflow/include/container/singleton.h"
#include "taskflow/include/work_manager/thread_pool.h"
#include "unistd.h"

using std::vector;
using taskflow::ThreadPool;
using AnyFunc = std::function<void(void)>;
namespace taskflow {
class WorkManager : public Singleton<WorkManager> {
 public:
  WorkManager() {
    pools_ = std::make_shared<ThreadPool>(sysconf(_SC_NPROCESSORS_ONLN));
  }
  ~WorkManager() noexcept {}

  auto Execute(AnyFunc task) { return pools_->enqueue(task); }

 private:
  std::shared_ptr<ThreadPool> pools_;
};
class WorkManagerWithNum {
 public:
  explicit WorkManagerWithNum(int num) {
    pools_ = std::make_shared<ThreadPool>(num);
  }
  ~WorkManagerWithNum() noexcept {}

  auto Execute(AnyFunc task) { return pools_->enqueue(task); }

 private:
  std::shared_ptr<ThreadPool> pools_;
};
}  // namespace taskflow
