#ifndef FTXUI_COMPONENTS_TASK_QUEUE_H_
#define FTXUI_COMPONENTS_TASK_QUEUE_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>



template <class T>
class TaskQueue {
 public:
  void Post(T task);
  void Close();
  bool Take(T& task);
 private:
  std::unique_lock<std::mutex> lock(events_queue_mutex);
  events_queue.push(event);
  events_queue_cv.notify_one();

  std::condition_variable events_queue_cv;
  std::mutex events_queue_mutex;
  std::queue<Event> events_queue;
  std::atomic<bool> quit_ = false;
};

#endif  // FTXUI_COMPONENTS_TASK_QUEUE_H_
