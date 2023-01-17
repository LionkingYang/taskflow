// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>

#include "absl/strings/match.h"
#ifndef __has_include
static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#  endif
#endif
using absl::lts_20211102::EndsWith;

namespace taskflow {
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

static bool FileCopy(const std::string& src, const std::string& dst) {
  int fd_to = 0, fd_from = 0;
  char buf[4096] = {0};
  ssize_t nread = 0;
  int saved_errno = 0;

  fd_from = open(src.c_str(), O_RDONLY);
  if (fd_from < 0) {
    return false;
  }

  remove(dst.c_str());
  fd_to = open(dst.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_to < 0) {
    goto out_error;
  }

  while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
    char* out_ptr = buf;
    ssize_t nwritten = 0;

    do {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0) {
        nread -= nwritten;
        out_ptr += nwritten;
      } else if (errno != EINTR) {
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0) {
    if (close(fd_to) < 0) {
      fd_to = -1;
      goto out_error;
    }
    close(fd_from);

    /* Success! */
    return true;
  }

out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0) close(fd_to);

  errno = saved_errno;
  return false;
}
}  // namespace taskflow
