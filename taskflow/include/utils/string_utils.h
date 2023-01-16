// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"

using std::string;
using std::vector;

namespace taskflow {
static inline std::unordered_map<std::string, std::string> split_twice(
    const std::string& s, std::string_view d1, std::string_view d2) {
  std::unordered_map<std::string, std::string> m;
  for (absl::string_view sp : absl::StrSplit(s, d1, absl::SkipEmpty())) {
    m.insert(absl::StrSplit(sp, absl::MaxSplits(d2, 1), absl::SkipEmpty()));
  }
  return m;
}

template <typename... T>
static inline void StrAppend(std::string* dest, const T&... args) {
  absl::StrAppend(dest, args...);
}

static inline bool HasPrefix(const string& word, const string& prefix) {
  return absl::StartsWith(word, prefix);
}

static inline bool Contains(const string& word, const string& needle) {
  return absl::StrContains(word, needle);
}

static inline vector<string> StrSplit(const string& word,
                                      const string& delimiter) {
  return absl::StrSplit(word, delimiter);
}

static inline vector<string> StrSplitByChars(const string& word,
                                             const string& delimter,
                                             bool skipEmpty = false) {
  if (skipEmpty)
    return absl::StrSplit(word, absl::ByAnyChar(delimter), absl::SkipEmpty());
  else
    return absl::StrSplit(word, absl::ByAnyChar(delimter));
}

}  // namespace taskflow
