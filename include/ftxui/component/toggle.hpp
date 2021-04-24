#ifndef FTXUI_COMPONENT_TOGGLE_H_
#define FTXUI_COMPONENT_TOGGLE_H_

#include <functional>
#include <string>

#include "ftxui/component/component.hpp"

namespace ftxui {

/// @brief An horizontal list of elements. The user can navigate through them.
/// @ingroup component
class Toggle : public Component {
 public:
  // Constructor.
  ~Toggle() override = default;

  // State.
  std::vector<std::wstring> entries = {L"On", L"Off"};
  int selected = 0;
  int focused = 0;

  Decorator normal_style = dim;
  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator selected_focused_style = focused_style | selected_style;

  // Callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  bool OnMouseEvent(Event event);
  std::vector<Box> boxes_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TOGGLE_H_ */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
