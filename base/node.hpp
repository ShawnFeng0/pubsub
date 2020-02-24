//
// Created by fs on 2/24/20.
//

#pragma once

#include "list.hpp"
#include "mutex.hpp"
#include <functional>

namespace PubSub {

typedef std::function<void(void)> CallbackPtr;

struct Callback : public ListNode<Callback *> {
  CallbackPtr call = nullptr;
};

template <typename T>
class Node {
protected:
  // The following data are unique in the same topic
  static T data_;
  static unsigned generation_;
  static List<Callback *> callbacks_;
  static Mutex lock_;
};

template <typename T>
T Node<T>::data_;

template <typename T>
unsigned Node<T>::generation_;

template <typename T>
List<Callback *> Node<T>::callbacks_;

template <typename T>
PubSub::Mutex Node<T>::lock_;

} // namespace PubSub
