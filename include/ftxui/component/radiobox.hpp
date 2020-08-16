#ifndef FTXUI_COMPONENT_RADIOBOX_HPP
#define FTXUI_COMPONENT_RADIOBOX_HPP

#include <functional>

#include "ftxui/component/component.hpp"

namespace ftxui {

/// @brief A list of selectable element. One and only one can be selected at
/// the same time.
/// @ingroup component
class RadioBox : public Component {
 public:
  // Constructor.
  RadioBox() = default;
  ~RadioBox() override = default;

  int selected = 0;
  int focused = 0;
  std::vector<std::wstring> entries;

#if defined(_WIN32)
  std::wstring checked = L"(*) ";
  std::wstring unchecked = L"( ) ";
#else
  std::wstring checked = L"◉ ";
  std::wstring unchecked = L"○ ";
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

#endif /* end of include guard: FTXUI_COMPONENT_RADIOBOX_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
