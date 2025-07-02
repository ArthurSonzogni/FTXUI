// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef TASK_RUNNER_HPP
#define TASK_RUNNER_HPP

#include "task.hpp"
#include "task_queue.hpp"

namespace ftxui::task {

class TaskRunner {
 public:
  TaskRunner();
  ~TaskRunner();

  // Returns the task runner for the current thread.
  static auto Current() -> TaskRunner*;

  /// Schedules a task to be executed immediately.
  auto PostTask(Task task) -> void;

  /// Schedules a task to be executed after a certain duration.
  auto PostDelayedTask(Task task,
                       std::chrono::steady_clock::duration duration) -> void;

  /// Runs the tasks in the queue, return the delay until the next delayed task
  /// can be executed.
  auto RunUntilIdle() -> std::optional<std::chrono::steady_clock::duration>;

  // Runs the tasks in the queue, blocking until all tasks are executed.
  auto Run() -> void;

 private:
  TaskRunner* previous_task_runner_ = nullptr;
  TaskQueue queue_;
};

} // namespace ftxui::task


#endif  // TASK_RUNNER_HPP
