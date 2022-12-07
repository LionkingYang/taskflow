// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include "spdlog/spdlog.h"

#define TASKFLOW_CRITICAL(...)                    \
  do {                                            \
    auto logger = spdlog::default_logger_raw();   \
    if (logger->should_log(spdlog::level::err)) { \
      SPDLOG_CRITICAL(__VA_ARGS__);               \
    }                                             \
                                                  \
  } while (0)

#define TASKFLOW_ERROR(...)                       \
  do {                                            \
    auto logger = spdlog::default_logger_raw();   \
    if (logger->should_log(spdlog::level::err)) { \
      SPDLOG_ERROR(__VA_ARGS__);                  \
    }                                             \
                                                  \
  } while (0)

#define TASKFLOW_INFO(...)                        \
  do {                                            \
    auto logger = spdlog::default_logger_raw();   \
    if (logger->should_log(spdlog::level::err)) { \
      SPDLOG_INFO(__VA_ARGS__);                   \
    }                                             \
                                                  \
  } while (0)
