// Copyright (c) 2022 liontyang<yangtian024@163.com> All rights reserved.
// Licensed under the Apache License. See License file in the project root for
// license information.

#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

#include "taskflow/include/container/singleton.h"
#include "taskflow/include/work_manager/work_manager.h"

namespace taskflow {
template <typename U>
struct HasClear {
  template <typename T, void (T::*)() = &T::Clear>
  static constexpr bool check(T*) {
    return true;
  }
  static constexpr bool check(...) { return false; }

  static constexpr bool ret = check(static_cast<U*>(0));
};

template <typename T, typename Enable = void>
class ObjectPool : public taskflow::Singleton<ObjectPool<T>> {
 public:
  explicit ObjectPool(size_t limit = 1024) : limit_(limit) {}
  void AddAvailable(T* v) {
    std::lock_guard<std::mutex> guard(alloc_mutex_);
    if (available_objs_.size() >= limit_) {
      delete v;
    } else {
      available_objs_.push_back(v);
    }
  }
  T* Get() {
    std::lock_guard<std::mutex> guard(alloc_mutex_);
    if (available_objs_.empty()) {
      return new T;
    }
    T* v = available_objs_.back();
    available_objs_.pop_back();
    return v;
  }
  void Recycle(T* v) {
    auto func = [=]() {
      if constexpr (HasClear<T>::ret) {
        v->Clear();
        this->AddAvailable(v);
      } else {
        v->~T();
        new (v) T;
        this->AddAvailable(v);
      }
    };
    WorkManager::GetInstance()->Execute(func);
  }

 private:
  std::deque<T*> available_objs_;
  std::mutex alloc_mutex_;
  size_t limit_;
};

template <typename T>
struct ObjectPool<T, typename std::enable_if<std::is_enum<T>::value || std::is_fundamental<T>::value>::type>
    : public taskflow::Singleton<ObjectPool<T>> {
  ObjectPool() {}

  T* Get() { return new T; }

  void Recycle(T* v) { delete v; }
};

template <typename T>
class PoolObjectGuard {
 public:
  PoolObjectGuard() {}
  T& Get() {
    if (nullptr != obj_) {
      return *obj_;
    }
    T* v = ObjectPool<T>::GetInstance()->Get();
    obj_ = v;
    return *v;
  }
  ~PoolObjectGuard() {
    if (nullptr != obj_) {
      ObjectPool<T>::GetInstance()->Recycle(obj_);
    }
  }

 private:
  T* obj_ = nullptr;
};

}  // namespace taskflow

#define GET_POOL_OBJ(Type, Ref)                   \
  taskflow::PoolObjectGuard<Type> _##Ref##_guard; \
  Type& Ref = _##Ref##_guard.Get();
