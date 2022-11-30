#include <thread>

#include "taskflow/include/async_task/async_task.h"

#define MAX_EVENTS_SIZE 1024
namespace taskflow {

TaskWorker::TaskWorker() {
  sem_init(&_sem, 0, 0);
  _running = true;
  _loop_thread = new std::thread([this]() { this->Run(); });
}
TaskWorker::~TaskWorker() {
  _running = false;
  sem_post(&_sem);
  _loop_thread->join();
  delete _loop_thread;
}
void TaskWorker::SetWaitTimeoutMicroSecs(int v) { _wait_timeout_us = v; }
void TaskWorker::SetInitTask(const AnyFunc &t) { _init_task = t; }
void TaskWorker::SetRoutineTask(const AnyFunc &t) { _routine_task = t; }
void TaskWorker::SetExitTask(const AnyFunc &t) { _exit_task = t; }

void TaskWorker::Run() {
  if (_init_task) {
    _init_task();
  }
  // struct epoll_event events[MAX_EVENTS_SIZE];
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
void TaskWorker::HandleTasks() {
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
void TaskWorker::Post(const AnyFunc &t, bool notify) {
  _queue.emplace(new AnyFunc(t));
  if (notify) {
    sem_post(&_sem);
  }
  // eventfd_write(_notify_fd, 1);
}
}  // namespace taskflow