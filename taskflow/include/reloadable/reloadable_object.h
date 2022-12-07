// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <sys/stat.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "taskflow/include/logger/logger.h"
#include "taskflow/include/reloadable/double_buffer.h"
#include "taskflow/include/work_manager/work_manager.h"

using namespace std::chrono_literals;

namespace taskflow {

template <typename T>
class ReloadableObj {
 public:
  explicit ReloadableObj(std::string path) : path_(path) {
    worker_ = std::make_shared<taskflow::WorkManager>();
    Load();
    running_ = true;
    Run();
  }

  ~ReloadableObj() { running_ = false; }
  bool Load() {
    struct stat st;
    if (stat(path_.c_str(), &st) != 0) {
      TASKFLOW_CRITICAL("Failed to stat file. path:{}", path_);
      return false;
    }

    if (st.st_mtim.tv_sec <= last_update_) {
      return true;
    }
    T& ptr_data = double_confs_.Next();
    if (!ptr_data.Init(path_)) {
      TASKFLOW_CRITICAL("init file failed:{}", path_);
      return false;
    }
    double_confs_.Switch();
    last_update_ = st.st_mtim.tv_sec;
    return true;
  }

  void Run() {
    auto sync_func = [this] {
      while (this->running_) {
        this->Load();
        std::this_thread::sleep_for(2000ms);
      }
    };
    worker_->Execute(sync_func);
  }

  inline const T& Get() { return double_confs_.Get(); }

 private:
  std::shared_ptr<taskflow::WorkManager> worker_;
  DoubleBuffer<T> double_confs_;
  bool running_ = false;
  std::string path_;
  int64_t last_update_ = 0;
};

}  // namespace taskflow
