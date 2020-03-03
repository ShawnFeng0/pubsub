//
// Created by fs on 2/24/20.
//

#pragma once

#include <stdint.h>

#include "base/node.hpp"

namespace PubSub {
template <typename T>
class SubscriptionData : private Callback, private Node<T> {
 public:
  SubscriptionData(const PubSub::CallbackPtr& cb = nullptr, bool instantly = false,
                   uint32_t interval_us = 0)
      : interval_us_(interval_us) {
    if (cb != nullptr) {
      RegisterCallback(cb, instantly);
    }
  }
  void RegisterCallback(const PubSub::CallbackPtr& cb, bool instantly = false) {
    MutexGuard lg(Node<T>::lock_);

    // Avoid duplicate additions
    Node<T>::callbacks_.remove(this);

    this->call = cb;
    Node<T>::callbacks_.add(this);

    // If data is already available and needs to be updated immediately
    if (instantly && Valid() && call) call();
  }

  bool Valid() {
    // Data at least once
    return Node<T>::generation_ != 0;
  }

  bool Update() {
    if (Node<T>::generation_ != last_generation_ &&
        GetAbsoluteTimeUs() - last_updated_time_us_ > interval_us_) {
      MutexGuard lg(Node<T>::lock_);
      last_generation_ = Node<T>::generation_;
      last_updated_time_us_ = GetAbsoluteTimeUs();
      data_ = Node<T>::data_;
      return true;
    }
    return false;
  }

  const T& get() const { return data_; }

  virtual ~SubscriptionData() { UnregisterCallback(); }

  void UnregisterCallback() {
    MutexGuard lg(Node<T>::lock_);
    Node<T>::callbacks_.remove(this);
  }

 private:
  static uint64_t GetAbsoluteTimeUs() {
    struct timespec ts {};
    uint64_t result;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    result = (uint64_t)(ts.tv_sec) * 1000000;
    result += ts.tv_nsec / 1000;

    return result;
  }

  unsigned last_generation_{0};
  uint32_t interval_us_{0};
  uint64_t last_updated_time_us_{0};
  T data_;
};
}  // namespace PubSub
