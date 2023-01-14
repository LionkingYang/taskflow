// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <string>

template <typename T>
class TaskFlowTrait {
 public:
  static const T get_default_v() {
    static T t;
    return t;
  }
};

template <>
class TaskFlowTrait<int> {
 public:
  static const int get_default_v() { return 0; }
};

template <>
class TaskFlowTrait<float> {
 public:
  static const float get_default_v() { return 0.0; }
};

template <>
class TaskFlowTrait<double> {
 public:
  static const double get_default_v() { return 0.0; }
};

template <>
class TaskFlowTrait<std::string> {
 public:
  static const std::string get_default_v() { return ""; }
};
