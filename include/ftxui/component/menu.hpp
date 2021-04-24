#ifndef FTXUI_COMPONENT_MENU
#define FTXUI_COMPONENT_MENU

#include <functional>

#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
class Menu : public Component {
 public:
  // Constructor.
  Menu() = default;
  ~Menu() override = default;

  // State.
  std::vector<std::wstring> entries = {};
  int selected = 0;
  int focused = 0;

  Decorator normal_style = nothing;
  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator selected_focused_style = focused_style | selected_style;

  // State update callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  bool OnMouseEvent(Event);

  std::vector<Box> boxes_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_MENU */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
