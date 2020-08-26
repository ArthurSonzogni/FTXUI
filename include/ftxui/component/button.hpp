#ifndef FTXUI_COMPONENT_BUTTON_HPP
#define FTXUI_COMPONENT_BUTTON_HPP

#include <functional>

#include "ftxui/component/component.hpp"

namespace ftxui {

/// @brief A button. An action is associated to the click event.
/// @ingroup dom
class Button : public Component {
 public:
  // Constructor.
  Button() = default;
  Button(std::wstring label) : label(label) {}
  ~Button() override = default;

  /// The Button label.
  std::wstring label = L"button";

  /// Called when the user press the "enter" button.
  std::function<void()> on_click = [] {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_BUTTON_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
