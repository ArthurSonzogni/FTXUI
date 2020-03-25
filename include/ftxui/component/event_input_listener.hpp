#ifndef FTXUI_COMPONENT_EVENT_INPUT_LISTENER_HPP
#define FTXUI_COMPONENT_EVENT_INPUT_LISTENER_HPP

#include <atomic>
#include <functional>
#include <thread>

#include "event.hpp"

#ifdef WIN32
  #include <condition_variable>
  #include <deque>
  #include <mutex>
#endif

namespace ftxui {

// Receives input events from the OS and turns them into
// Event objects and sends to a consumer
//
// On NIX systems:
//   - uses SIGWINCH for resize
//   - uses getchar() for keypresses
//
// On Windows systems:
//   - Uses ReadConsoleInput for resize and keypresses
class EventInputListener {
 public:
  EventInputListener(std::function<void(Event)> consumer);
  ~EventInputListener();

  void stop();

 private:
  char readchar();
  void readchar_thread_func(std::function<void(Event)> consumer);

  std::atomic<bool> quit_{false};
  std::thread readchar_thread_;
#ifndef _WIN32
  using signal_handler_t = void (*)(int);
  signal_handler_t old_sigwinch_handler_;
#else
  void input_thread_func(std::function<void(Event)> consumer);

  std::mutex input_queue_mutex_;
  std::condition_variable input_queue_condvar_;
  std::deque<char> input_queue_;
  std::thread input_event_thread_;
#endif
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_EVENT_INPUT_LISTENER_HPP */
