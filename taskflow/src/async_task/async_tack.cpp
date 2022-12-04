// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#include <thread>

#include "taskflow/include/async_task/async_task.h"

#define MAX_EVENTS_SIZE 1024
namespace taskflow {

AsyncWorker::AsyncWorker() {
  sem_init(&_sem, 0, 0);
  _running = true;
  _loop_thread = new std::thread([this]() { this->Run(); });
}
AsyncWorker::~AsyncWorker() {
  _running = false;
  sem_post(&_sem);
  _loop_thread->join();
  delete _loop_thread;
}
void AsyncWorker::SetWaitTimeoutMicroSecs(int v) { _wait_timeout_us = v; }
void AsyncWorker::SetInitTask(const AnyFunc &t) { _init_task = t; }
void AsyncWorker::SetRoutineTask(const AnyFunc &t) { _routine_task = t; }
void AsyncWorker::SetExitTask(const AnyFunc &t) { _exit_task = t; }

void AsyncWorker::Run() {
  if (_init_task) {
    _init_task();
  }
  while (_running) {
    HandleTasks();
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += (_wait_timeout_us * 1000);
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;
    sem_timedwait(&_sem, &ts);
  }
  if (_exit_task) {
    _exit_task();
  }
}
void AsyncWorker::HandleTasks() {
  if (_routine_task) {
    _routine_task();
  }
  while (1) {
    AnyFunc *t = nullptr;
    if (_queue.try_pop(t)) {
      (*t)();
      delete t;
    } else {
      break;
    }
  }
}
void AsyncWorker::Post(const AnyFunc &t, bool notify) {
  _queue.emplace(new AnyFunc(t));
  if (notify) {
    sem_post(&_sem);
  }
}
}  // namespace taskflow
