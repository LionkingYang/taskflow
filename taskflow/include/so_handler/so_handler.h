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
#include "taskflow/include/utils/class_helper.h"
#include "taskflow/include/work_manager/work_manager.h"

namespace taskflow {

class SoScript {
 public:
  using SymboleTable = std::unordered_map<std::string, void*>;

  SoScript() : worker_(std::make_shared<taskflow::WorkManager>()) {}
  ~SoScript();

  const taskflow::TaskFunc GetFunc(const std::string& func);

  bool Init(const std::string& so_path);

 private:
  std::string so_path_;

  std::mutex cache_mutex_;
  SymboleTable cache_syms_;

  void* so_handler_ = nullptr;
  std::shared_ptr<taskflow::WorkManager> worker_;
  DISALLOW_COPY_AND_ASSIGN(SoScript);
};

using SoScriptPtr = std::shared_ptr<SoScript>;

}  // namespace taskflow
