//
// Created by fs on 2/24/20.
//

#pragma once

#include "base/node.hpp"

namespace PubSub {
template <typename T> class SubscriptionData : public Callback, private Node<T> {
public:
  void RegisterCallback(const PubSub::CallbackPtr& cb) {
    MutexGuard lg(Node<T>::lock_);

    // Avoid duplicate additions
    Node<T>::callbacks_.remove(this);

    this->call = cb;
    Node<T>::callbacks_.add(this);
  }

  bool Valid() {
    // Data at least once
    return Node<T>::generation_ != 0;
  }

  bool Update() {
    if (Node<T>::generation_ != last_generation_) {
      MutexGuard lg(Node<T>::lock_);
      last_generation_ = Node<T>::generation_;
      data_ = Node<T>::data_;
      return true;
    }
    return false;
  }

  const T &get() const { return data_; }

  virtual ~SubscriptionData() { UnregisterCallback(); }

  void UnregisterCallback() {
    MutexGuard lg(Node<T>::lock_);
    Node<T>::callbacks_.remove(this);
  }

private:
  unsigned last_generation_{0};
  T data_;
};
} // namespace PubSub
