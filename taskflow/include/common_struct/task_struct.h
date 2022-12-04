// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "taskflow/include/kcfg/kcfg.h"
using std::string;
using std::unordered_map;
using std::vector;

struct Job {
  string task_name;
  vector<string> dependencies;
  KCFG_DEFINE_FIELDS(task_name, dependencies)
};

struct Jobs {
  vector<Job> tasks;
  KCFG_DEFINE_FIELDS(tasks)
};
