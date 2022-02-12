#include <functional>
#include <variant>
#include "ftxui/component/event.hpp"

namespace ftxui {
using Closure = std::function<void()>;
using Task = std::variant<Event, Closure>;
}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
