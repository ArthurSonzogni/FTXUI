#ifndef FTXUI_COMPONENT_ANIMATION_HPP
#define FTXUI_COMPONENT_ANIMATION_HPP

#include <functional>
#include <variant>
#include "ftxui/component/event.hpp"

namespace ftxui {
class AnimationTask {};
using Closure = std::function<void()>;
using Task = std::variant<Event, Closure, AnimationTask>;
}  // namespace ftxui

#endif  // FTXUI_COMPONENT_ANIMATION_HPP

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
