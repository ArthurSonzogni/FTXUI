// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef TASK_QUEUE_HPP
#define TASK_QUEUE_HPP

#include <queue>
#include <variant>

#include "ftxui/component/task_internal.hpp"  // for PendingTask, Task

namespace ftxui::task {

/// A task queue that schedules tasks to be executed in the future. Tasks can be
/// scheduled to be executed immediately, or after a certain duration.
/// - The tasks are executed in the order they were scheduled.
/// - If multiple tasks are scheduled to be executed at the same time, they are
///   executed in the order they were scheduled.
/// - If a task is scheduled to be executed in the past, it is executed
///   immediately.
struct TaskQueue {
  auto PostTask(PendingTask task) -> void;

  using MaybeTask =
      std::variant<Task, std::chrono::steady_clock::duration, std::monostate>;
  auto Get() -> MaybeTask;

  bool HasImmediateTasks() const { return !immediate_tasks_.empty(); }

 private:
  std::queue<PendingTask> immediate_tasks_;
  std::priority_queue<PendingTask> delayed_tasks_;
};

}  // namespace ftxui::task

#endif
