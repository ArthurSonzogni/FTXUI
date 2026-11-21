// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_TASK_HPP
#define FTXUI_COMPONENT_TASK_HPP

#include <functional>
#include <variant>
#include "ftxui/component/event.hpp"

namespace ftxui {
/// Marker task requesting an animation step.
class AnimationTask {};
/// Zero-argument callback executed on the App main loop.
using Closure = std::function<void()>;
/// Unit of work queued on the App event loop.
using Task = std::variant<Event, Closure, AnimationTask>;
}  // namespace ftxui

#endif  // FTXUI_COMPONENT_TASK_HPP
