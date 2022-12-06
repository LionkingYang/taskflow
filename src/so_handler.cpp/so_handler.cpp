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
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

#include "absl/strings/match.h"
#include "taskflow/include/reloadable/reloadable_object.h"

using absl::lts_20211102::EndsWith;
using namespace std::chrono_literals;

bool ListDir(const std::string& path, std::vector<std::string>& dirs) {
  struct stat buf = {0};
  int ret = stat(path.c_str(), &buf);
  if (0 == ret) {
    if (S_ISDIR(buf.st_mode)) {
      DIR* dir = opendir(path.c_str());
      if (nullptr != dir) {
        struct dirent* ptr = nullptr;
        while ((ptr = readdir(dir)) != nullptr) {
          if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
            continue;
          }
          std::string file_path = path;
          file_path.append("/").append(ptr->d_name);
          memset(&buf, 0, sizeof(buf));
          ret = stat(path.c_str(), &buf);
          if (ret == 0) {
            if (S_ISDIR(buf.st_mode)) {
              dirs.push_back(ptr->d_name);
            }
          }
        }
        closedir(dir);
        return true;
      }
    }
  }
  return false;
}

namespace taskflow {

const taskflow::TaskFunc SoScript::GetFunc(const std::string& func) {
  std::lock_guard<std::mutex> guard(cache_mutex_);
  auto found = cache_syms_.find(func);
  if (found != cache_syms_.end()) {
    return reinterpret_cast<taskflow::TaskFunc>(found->second);
  }

  void* sym = dlsym(so_handler_, func.c_str());
  cache_syms_[func] = sym;
  return reinterpret_cast<taskflow::TaskFunc>(sym);
}

bool SoScript::Init(const std::string& so_path) {
  so_path_ = so_path;
  std::vector<string> dirs;
  ListDir(so_path_, dirs);
  string max_so = "";
  int64_t max_m = 0;
  for (const auto& each : dirs) {
    if (EndsWith(each, "so")) {
      std::string so = so_path + "/" + each;
      struct stat st;
      if (stat(so.c_str(), &st) != 0) {
        continue;
      }

      if (st.st_mtim.tv_sec >= max_m) {
        max_m = st.st_mtim.tv_sec;
        max_so = so;
      }
    }
  }
  cache_syms_.clear();
  so_handler_ = dlopen(max_so.c_str(), RTLD_NOW);
  if (nullptr == so_handler_) {
    std::cout << "open dl error:" << max_so << std::endl;
    return false;
  }

  return true;
}

SoScript::~SoScript() {
  // delay release
  void* handler = so_handler_;
  std::string path = so_path_;
  //   worker_->Execute([handler, path]() {
  //     // sleep 1min
  //     std::this_thread::sleep_for(60 * 1000ms);
  if (nullptr != handler) {
    if (0 != dlclose(handler)) {
      std::cout << "ERROR in close handler\n";
    }
  }
  //     if (!path.empty()) {
  //       remove(path.c_str());
  //     }
  //   });
}

}  // namespace taskflow
