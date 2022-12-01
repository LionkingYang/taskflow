#pragma once

#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "oneapi/tbb/concurrent_queue.h"

using AnyFunc = std::function<void(void)>;

namespace taskflow {
class TaskWorker {
 public:
 private:
  std::thread *_loop_thread = nullptr;
  tbb::concurrent_queue<AnyFunc *> _queue;
  sem_t _sem;
  int _wait_timeout_us = 10000;
  AnyFunc _routine_task;
  AnyFunc _init_task;
  AnyFunc _exit_task;
  bool _running = false;
  void Run();
  void HandleTasks();

 public:
  TaskWorker();
  void SetInitTask(const AnyFunc &t);
  void SetRoutineTask(const AnyFunc &t);
  void SetExitTask(const AnyFunc &t);
  void SetWaitTimeoutMicroSecs(int v);
  void Post(const AnyFunc &t, bool notify);
  ~TaskWorker();
};
}  // namespace taskflow
