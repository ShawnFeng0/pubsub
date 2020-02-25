//
// Created by fs on 2/24/20.
//

#pragma once

#include "base/mutex.hpp"
#include "base/node.hpp"

namespace PubSub {
template <typename T> class PublicationData : private Node<T> {
public:

  void Publish() {
    MutexGuard lg(Node<T>::lock_);
    Node<T>::data_ = data_;
    Node<T>::generation_++;
    for (auto item : Node<T>::callbacks_) {
      if (item->call)
        item->call();
    }
  }

  T &get() { return data_; }

  auto set(const T &data) {
    data_ = data;
    return this;
  }

private:
  T data_;
};
} // namespace PubSub
