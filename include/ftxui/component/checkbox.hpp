// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_COMPONENT_CHECKBOX_HPP
#define FTXUI_COMPONENT_CHECKBOX_HPP

#include <functional>

#include "ftxui/component/component.hpp"

namespace ftxui {

class CheckBox : public Component {
 public:
  // Constructor.
  CheckBox() = default;
  ~CheckBox() override = default;

  bool state = false;
  std::wstring label = L"label";

#if defined(_WIN32)
  std::wstring checked = L"[X] ";
  std::wstring unchecked = L"[ ] ";
#else
  std::wstring checked = L"▣ ";
  std::wstring unchecked = L"☐ ";
#endif

  Decorator focused_style = inverted;
  Decorator unfocused_style = nothing;

  // State update callback.
  std::function<void()> on_change = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  int cursor_position = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_CHECKBOX_HPP */
