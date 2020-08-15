#include "ftxui/component/checkbox.hpp"

#include <functional>

namespace ftxui {

Element CheckBox::Render() {
  bool is_focused = Focused();
  auto style = is_focused ? focused_style : unfocused_style;
  auto focus_management = is_focused ? focus : state ? select : nothing;
  return hbox(text(state ? checked : unchecked),
              text(label) | style | focus_management);
}

bool CheckBox::OnEvent(Event event) {
  if (event == Event::Character(' ') || event == Event::Return) {
    state = !state;
    on_change();
    return true;
  }
  return false;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
