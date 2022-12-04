// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <atomic>

template <typename T>
class DoubleBuffer {
 public:
  const T& Get() { return data[index_.load()]; }
  void Switch() { index_.fetch_xor(1); }
  T& set() { return data[1 - index_.load()]; }

 private:
  T data[2];
  std::atomic<int> index_;
};
