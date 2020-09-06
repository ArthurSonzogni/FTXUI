#include "ftxui/component/button.hpp"

#include <functional>

namespace ftxui {

Element Button::Render() {
  if (Focused())
    return text(label) | border | inverted;
  else
    return text(label) | border;
}

bool Button::OnEvent(Event event) {
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
