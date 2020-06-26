#ifndef BUFFER_H
#define BUFFER_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <class T> class Buffer {
public:
  Buffer(size_t len) : _len(len){};
  void push(T elem);
  bool pop(T &elem);

private:
  // do not use default constractor
  Buffer();
  const size_t _len;
  std::queue<T> q;
  std::mutex _mtx;
  std::condition_variable _condition;
};

template <class T> void Buffer<T>::push(T elem) {
  std::lock_guard<std::mutex> lock(_mtx);
  if (q.size() >= _len) {
    q.pop();
  }
  q.emplace(elem);
  _condition.notify_one();
}

template <class T> bool Buffer<T>::pop(T &elem) {
  std::unique_lock<std::mutex> lock(_mtx);
  // Wait for a new object for 1000 ms at most
  _condition.wait_until(
      lock, std::chrono::system_clock::now() + std::chrono::milliseconds(1000),
      [this] { return !q.empty(); });
  if (!q.empty()) {
    elem = q.front();
    q.pop();
    return true;
  }
  return false;
}

#endif