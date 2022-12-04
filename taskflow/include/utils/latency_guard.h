// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
// Usage:
// {
//    LatencyGuard latency("some function");
//    do_somethings();
// }

#include <chrono>
#include <iostream>
#include <string>

#include "taskflow/include/utils/class_helper.h"

namespace taskflow {
class LatencyGuard {
 public:
  explicit LatencyGuard(const std::string& func_name) : func_name_(func_name) {
    start_time_ = std::chrono::high_resolution_clock::now();
  }

  ~LatencyGuard() {
    std::cout << func_name_ << " cost"
              << std::chrono::duration_cast<
                     std::chrono::duration<double, std::milli>>(
                     std::chrono::high_resolution_clock::now() - start_time_)
                     .count()
              << "ms\n";
  }

 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  std::string func_name_;
  DISALLOW_COPY_AND_ASSIGN(LatencyGuard)
};
}  // namespace taskflow
