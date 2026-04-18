#pragma once

#include "common.hpp"
#include <array>

template <typename T, u8 N> class Queue {
public:
  std::array<T, N> data;
  u8 head;
  u8 tail;

  Queue() : head(0), tail(0) {};

  void enqueue(const T &t) {
    data[tail] = t;
    tail++;
    if (tail == N) {
      tail = 0;
    }
  }

  T dequeue() {
    T &value = data[head];
    head++;
    if (head == N) {
      head = 0;
    }
    return value;
  }

  bool empty() const { return head == tail; }
};