// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;
namespace taskflow {
static void init_loggers(const std::string &dir, const std::string &level,
                         bool logstderr) {
  spdlog::init_thread_pool(8192, 1);
  if (!fs::exists(dir)) fs::create_directories(dir);
  auto log_level = spdlog::level::from_str(level);
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      dir + "/taskflow.log", 100 * 1024 * 1024, 10);
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(file_sink);
  if (logstderr) {
    auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    sinks.push_back(console_sink);
  }
  auto main_logger = std::make_shared<spdlog::async_logger>(
      "main_log", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);
  main_logger->set_level(log_level);
  main_logger->set_pattern(
      "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%P/%t] [%s:%#] %v");
  main_logger->flush_on(log_level);
  spdlog::register_logger(main_logger);
  spdlog::set_default_logger(spdlog::get("main_log"));
  spdlog::flush_every(std::chrono::seconds(3));
}
}  // namespace taskflow

#define TASKFLOW_CRITICAL(...)                         \
  do {                                                 \
    auto logger = spdlog::default_logger_raw();        \
    if (logger->should_log(spdlog::level::critical)) { \
      SPDLOG_CRITICAL(__VA_ARGS__);                    \
    }                                                  \
  } while (0)

#define TASKFLOW_ERROR(...)                       \
  do {                                            \
    auto logger = spdlog::default_logger_raw();   \
    if (logger->should_log(spdlog::level::err)) { \
      SPDLOG_ERROR(__VA_ARGS__);                  \
    }                                             \
  } while (0)

#define TASKFLOW_INFO(...)                         \
  do {                                             \
    auto logger = spdlog::default_logger_raw();    \
    if (logger->should_log(spdlog::level::info)) { \
      SPDLOG_INFO(__VA_ARGS__);                    \
    }                                              \
  } while (0)

#define TASKFLOW_DEBUG(...)                         \
  do {                                              \
    auto logger = spdlog::default_logger_raw();     \
    if (logger->should_log(spdlog::level::debug)) { \
      SPDLOG_DEBUG(__VA_ARGS__);                    \
    }                                               \
  } while (0)

#define TASKFLOW_TRACE(...)                         \
  do {                                              \
    auto logger = spdlog::default_logger_raw();     \
    if (logger->should_log(spdlog::level::trace)) { \
      SPDLOG_TRACE(__VA_ARGS__);                    \
    }                                               \
  } while (0)

#define TASKFLOW_WARN(...)                         \
  do {                                             \
    auto logger = spdlog::default_logger_raw();    \
    if (logger->should_log(spdlog::level::warn)) { \
      SPDLOG_WARN(__VA_ARGS__);                    \
    }                                              \
  } while (0)
