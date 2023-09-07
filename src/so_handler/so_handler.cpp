// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include "taskflow/include/so_handler/so_handler.h"

#include <dirent.h>
#include <dlfcn.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

#include "taskflow/include/logger/logger.h"
#include "taskflow/include/reloadable/reloadable_object.h"
#include "taskflow/include/utils/file_helper.h"
#include "taskflow/include/utils/string_utils.h"
#include "taskflow/include/utils/time_helper.h"

using namespace std::chrono_literals;
constexpr char kSoPrefix[] = "so_on_use_";

namespace taskflow {

const taskflow::TaskFunc SoScript::GetFunc(const std::string& func) {
  {
    std::shared_lock lock(mutex_);
    if (auto func_ptr = cache_syms_.find_with_value(func);
        func_ptr != nullptr) {
      return reinterpret_cast<taskflow::TaskFunc>(*func_ptr);
    }
  }
  std::unique_lock lock(mutex_);
  if (auto func_ptr = cache_syms_.find_with_value(func); func_ptr != nullptr) {
    return reinterpret_cast<taskflow::TaskFunc>(*func_ptr);
  }

  void* sym = dlsym(so_handler_, func.c_str());
  cache_syms_[func] = sym;
  return reinterpret_cast<taskflow::TaskFunc>(sym);
}

void SoScript::Run() {
  auto func = [this]() {
    while (running_) {
      this->Reload();
      std::this_thread::sleep_for(5000ms);
    }
  };
  t_ = std::thread(func);
}

bool SoScript::Reload() {
  std::vector<string> dirs;
  taskflow::ListDir(so_path_, dirs);
  string max_so = "";
  int64_t max_m = 0;
  for (const auto& each : dirs) {
    if (EndsWith(each, "so")) {
      std::string so;
      taskflow::StrAppend(&so, so_path_, "/", each);
      struct stat st;
      if (stat(so.c_str(), &st) != 0) {
        continue;
      }

      if (st.st_mtim.tv_sec >= max_m) {
        if (!max_so.empty()) remove(max_so.c_str());
        max_m = st.st_mtim.tv_sec;
        max_so = so;
      } else {
        remove(so.c_str());
      }
    }
  }
  if (max_m <= last_update_) {
    TASKFLOW_INFO("no update of so file");
    return true;
  }
  last_update_ = max_m;
  cache_syms_.clear();
  std::string new_so;
  taskflow::StrAppend(&new_so, so_path_, "/", kSoPrefix, TNOWMS, ".so");
  rename(max_so.c_str(), new_so.c_str());
  so_handler_ = dlopen(new_so.c_str(), RTLD_NOW);
  if (nullptr == so_handler_) {
    TASKFLOW_CRITICAL("open dl error:{}, {}", new_so, dlerror());
    return false;
  }

  return true;
}

SoScript::~SoScript() noexcept {
  // delay release
  running_ = false;
  t_.join();
  void* handler = so_handler_;
  if (nullptr != handler) {
    if (0 != dlclose(handler)) {
      TASKFLOW_CRITICAL("ERROR in close handler");
    }
  }
}

}  // namespace taskflow
