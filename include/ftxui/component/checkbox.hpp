#ifndef FTXUI_COMPONENT_CHECKBOX_HPP
#define FTXUI_COMPONENT_CHECKBOX_HPP

#include <functional>

#include "ftxui/component/component.hpp"

namespace ftxui {

/// @brief A Checkbox. It can be checked or unchecked.Display an element on a
/// ftxui::Screen.
/// @ingroup dom
class CheckBox : public Component {
 public:
  // Constructor.
  CheckBox() = default;
  ~CheckBox() override = default;

  bool state = false;  // The current state. true=checked, false:unchecked.
  std::wstring label = L"label";  // The CheckBox label.

#if defined(_WIN32)
  std::wstring checked = L"[X] ";    /// Prefix for  a "checked" state.
  std::wstring unchecked = L"[ ] ";  /// Prefix for  an "unchecked" state.
#else
  std::wstring checked = L"▣ ";    /// Prefix for  a "checked" state.
  std::wstring unchecked = L"☐ ";  /// Prefix for  a "unchecked" state.
#endif

  Decorator focused_style = inverted;   /// Decorator used when focused.
  Decorator unfocused_style = nothing;  /// Decorator used when unfocused.

  /// Called when the user change the state of the CheckBox.
  std::function<void()> on_change = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  int cursor_position = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_CHECKBOX_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
