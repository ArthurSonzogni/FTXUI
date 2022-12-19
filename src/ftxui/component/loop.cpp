#include "ftxui/component/loop.hpp"

#include <utility>  // for move

#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive, Component

namespace ftxui {

// NOLINTNEXTLINE
Loop::Loop(ScreenInteractive* screen, Component component)
    : screen_(screen), component_(std::move(component)) {
  screen_->PreMain();
}

Loop::~Loop() {
  screen_->PostMain();
}

bool Loop::HasQuitted() {
  return screen_->HasQuitted();
}

/// @brief Execute the loop. Make the `component` to process every pending
/// tasks/events. A new frame might be drawn if the previous was invalidated.
/// Return true until the loop hasn't completed.
void Loop::RunOnce() {
  screen_->RunOnce(component_);
}

/// @brief Wait for at least one event to be handled and execute
/// `Loop::RunOnce()`.
void Loop::RunOnceBlocking() {
  screen_->RunOnceBlocking(component_);
}

/// Execute the loop, blocking the current thread, up until the loop has
/// quitted.
void Loop::Run() {
  while (!HasQuitted()) {
    RunOnceBlocking();
  }
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
