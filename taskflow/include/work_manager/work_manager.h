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

using std::vector;
using AnyFunc = std::function<void(void)>;
namespace taskflow {
class WorkManager : public Singleton<WorkManager> {
 public:
  WorkManager() { pools_ = std::make_shared<ThreadPool>(4); }
  ~WorkManager() noexcept {}

  void Execute(AnyFunc task) { pools_->enqueue(task); }

 private:
  std::shared_ptr<ThreadPool> pools_;
};
}  // namespace taskflow
