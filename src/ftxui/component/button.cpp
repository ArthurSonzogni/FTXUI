#include "ftxui/component/button.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include <functional>

namespace ftxui {

Element Button::Render() {
  auto style = Focused() ? inverted : nothing;
  return text(label) | border | style | reflect(box_);
}

bool Button::OnEvent(Event event) {
  if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y)) {
    if (!event.screen()->CaptureMouse())
      return false;

    TakeFocus();

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      on_click();
      return true;
    }

    return false;
  }

  if (event == Event::Return) {
    on_click();
    return true;
  }
  return false;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
