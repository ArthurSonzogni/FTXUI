// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/task_queue.hpp"

namespace ftxui::task {

auto TaskQueue::PostTask(PendingTask task) -> void {
  const std::lock_guard<std::mutex> lock(mutex_);
  if (!task.time) {
    immediate_tasks_.push(task);
    return;
  }

  if (task.time.value() < std::chrono::steady_clock::now()) {
    immediate_tasks_.push(task);
    return;
  }

  delayed_tasks_.push(task);
}

auto TaskQueue::Get() -> MaybeTask {
  const std::lock_guard<std::mutex> lock(mutex_);
  // Attempt to execute a task immediately.
  if (!immediate_tasks_.empty()) {
    auto task = immediate_tasks_.front();
    immediate_tasks_.pop();
    return task.task;
  }

  // Move all tasks that can be executed to the immediate queue.
  auto now = std::chrono::steady_clock::now();
  while (!delayed_tasks_.empty()) {
    const auto& top = delayed_tasks_.top();
    if (!top.time.has_value() || top.time.value() > now) {
      break;
    }
    immediate_tasks_.push(top);
    delayed_tasks_.pop();
  }

  // Attempt to execute a task immediately.
  if (!immediate_tasks_.empty()) {
    auto task = immediate_tasks_.front();
    immediate_tasks_.pop();
    return task.task;
  }

  // If there are no tasks to execute, return the delay until the next task.
  if (!delayed_tasks_.empty()) {
    const auto& top = delayed_tasks_.top();
    if (top.time.has_value()) {
      return top.time.value() - now;
    }
  }

  // If there are no tasks to execute, return the maximum duration.
  return std::monostate{};
}

auto TaskQueue::HasImmediateTasks() const -> bool {
  const std::lock_guard<std::mutex> lock(mutex_);
  return !immediate_tasks_.empty();
}

}  // namespace ftxui::task
