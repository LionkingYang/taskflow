// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <mutex>

namespace taskflow {

template <typename T>
class CreateUsingNew {
 public:
  static T* Create() { return new T; }

  static void Destroy(T* t) { delete t; }
};

template <typename T>
class CreateStatic {
 public:
  union MaxAlign {
    char t_[sizeof(T)];
    long double longDouble_;
  };

  static T* Create() {
    static MaxAlign t;
    return new (&t) T;
  }

  static void Destroy(T* t) { t->~T(); }
};

template <typename T>
class DefaultLifetime {
 public:
  static void DeadReference() { assert(false); }

  static void ScheduleDestruction(T*, void (*pFun)()) { std::atexit(pFun); }
};

template <typename T>
class PhoneixLifetime {
 public:
  static void DeadReference() { destroyed_once_ = true; }

  static void ScheduleDestruction(T*, void (*pFun)()) {
    if (!destroyed_once_) {
      std::atexit(pFun);
    }
  }

 private:
  static bool destroyed_once_;
};
template <class T>
bool PhoneixLifetime<T>::destroyed_once_ = false;

template <typename T>
struct NoDestroyLifetime {
  static void ScheduleDestruction(T*, void (*)()) {}

  static void DeadReference() {}
};

template <typename T, template <class> class CreatePolicy = CreateUsingNew,
          template <class> class LifetimePolicy = DefaultLifetime>
class Singleton {
 public:
  typedef T instance_type;
  typedef volatile T volatile_type;

  static T* GetInstance() {
    T* tmp = instance_.load(std::memory_order_relaxed);
    if (tmp == nullptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      tmp = instance_.load(std::memory_order_relaxed);
      if (tmp == nullptr) {
        if (destroyed_.load(std::memory_order_acquire)) {
          LifetimePolicy<T>::DeadReference();
          destroyed_.store(false, std::memory_order_release);
        }
        tmp = CreatePolicy<T>::Create();
        std::atomic_thread_fence(std::memory_order_release);
        instance_.store(tmp, std::memory_order_relaxed);
        LifetimePolicy<T>::ScheduleDestruction(tmp, &DestroySingleton);
      }
    }
    return tmp;
  }

  virtual ~Singleton() {}

 protected:
  static void DestroySingleton() {
    assert(!destroyed_.load(std::memory_order_acquire));
    CreatePolicy<T>::Destroy(instance_.load(std::memory_order_acquire));
    instance_.store(nullptr, std::memory_order_release);
    destroyed_.store(true, std::memory_order_release);
  }

 protected:
  static std::mutex mutex_;
  static std::atomic<T*> instance_;
  static std::atomic<bool> destroyed_;

 protected:
  Singleton() {}
  Singleton(const Singleton&);
  Singleton& operator=(const Singleton&);
};

template <class T, template <class> class CreatePolicy,
          template <class> class LifetimePolicy>
std::mutex Singleton<T, CreatePolicy, LifetimePolicy>::mutex_;

template <class T, template <class> class CreatePolicy,
          template <class> class LifetimePolicy>
std::atomic<bool> Singleton<T, CreatePolicy, LifetimePolicy>::destroyed_{false};

template <class T, template <class> class CreatePolicy,
          template <class> class LifetimePolicy>
std::atomic<T*> Singleton<T, CreatePolicy, LifetimePolicy>::instance_{nullptr};

}  // namespace taskflow
