// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/common_struct/task_struct.h"
#include "taskflow/include/container/concurrent_map.h"
#include "taskflow/include/container/singleton.h"
#include "taskflow/include/utils/class_helper.h"
#include "taskflow/include/work_manager/work_manager.h"

namespace taskflow {

class SoScript : taskflow::Singleton<SoScript> {
 public:
  using SymboleTable = taskflow::ConcurrentMap<std::string, void*>;

  explicit SoScript(const std::string& so_path) : so_path_(so_path) {
    running_ = true;
    Reload();
    Run();
  }
  ~SoScript() noexcept;

  const taskflow::TaskFunc GetFunc(const std::string& func);

 private:
  void Run();
  bool Reload();
  int running_ = false;
  std::string so_path_;
  tbb::task_group worker_;
  SymboleTable cache_syms_;
  int64_t last_update_ = 0;
  void* so_handler_ = nullptr;
  std::thread t_;
};

using SoScriptPtr = std::shared_ptr<SoScript>;

}  // namespace taskflow
