// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/task_internal.hpp"

namespace ftxui::task {
bool PendingTask::operator<(const PendingTask& other) const {
  if (!time && !other.time) {
    return false;
  }
  if (!time) {
    return true;
  }
  if (!other.time) {
    return false;
  }
  return time.value() > other.time.value();
}
}  // namespace ftxui::task
