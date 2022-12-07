// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>

#include "absl/strings/match.h"
using absl::lts_20211102::EndsWith;

static bool ListDir(const std::string& path, std::vector<std::string>& dirs) {
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