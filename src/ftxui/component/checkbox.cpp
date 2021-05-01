#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include <functional>

namespace ftxui {

Element CheckBox::Render() {
  bool is_focused = Focused();
  auto style = is_focused ? focused_style : unfocused_style;
  auto focus_management = is_focused ? focus : state ? select : nothing;
  return hbox(text(state ? checked : unchecked),
              text(label) | style | focus_management) |
         reflect(box_);
}

bool CheckBox::OnEvent(Event event) {
  if (event.is_mouse())
    return OnMouseEvent(event);

  if (event == Event::Character(' ') || event == Event::Return) {
    state = !state;
    on_change();
    return true;
  }
  return false;
}

bool CheckBox::OnMouseEvent(Event event) {
  if (!event.screen()->CaptureMouse())
    return false;
  if (!box_.Contain(event.mouse().x, event.mouse().y))
    return false;

  TakeFocus();

  if (event.mouse().button == Mouse::Left &&
      event.mouse().motion == Mouse::Pressed) {
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
