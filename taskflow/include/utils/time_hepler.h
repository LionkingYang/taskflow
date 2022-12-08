// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.
#pragma once

#include <limits.h>

#include "absl/time/clock.h"

// static const uint32 kmax_uint32 = 0xffffffff;
namespace taskflow {

static int64_t GetCurrentTimeNanos() { return absl::GetCurrentTimeNanos(); }

static int64_t GetCurrentTimeMicros() { return GetCurrentTimeNanos() / 1000L; }

static int64_t GetCurrentTimeMillis() {
  return GetCurrentTimeNanos() / 1000000L;
}

static int64_t GetCurrentTimeSeconds() {
  return GetCurrentTimeNanos() / 1000000000L;
}

static int64_t GetCurrentTimeMinutes() { return GetCurrentTimeSeconds() / 60; }

static int64_t GetCurrentTimeHours() { return GetCurrentTimeSeconds() / 3600; }
}  // namespace taskflow
