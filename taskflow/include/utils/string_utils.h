// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "absl/strings/str_split.h"

static std::unordered_map<std::string, std::string> split_twice(
    const std::string& s, std::string_view d1, std::string_view d2) {
  std::unordered_map<std::string, std::string> m;
  for (absl::string_view sp : absl::StrSplit(s, d1)) {
    m.insert(absl::StrSplit(sp, absl::MaxSplits(d2, 1)));
  }
  return m;
}