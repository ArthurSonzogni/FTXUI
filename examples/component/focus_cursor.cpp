#include <memory>  // for allocator, shared_ptr, __shared_ptr_access
#include <string>  // for operator+, char_traits, to_string, string
#include <vector>  // for vector

#include "ftxui/component/component.hpp"       // for Slider, Renderer, Vertical
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Elements, Element, operator|, separator, text, focusPositionRelative, size, border, flex, frame, bgcolor, gridbox, vbox, EQUAL, center, HEIGHT, WIDTH

using namespace ftxui;

Component Instance(std::string label, Decorator focusCursor) {
  return Renderer([=](bool focused) {
    if (focused) {
      return hbox({
          text("> " + label + " "),
          focusCursor(text(" ")),
      });
    }
    return text("  " + label + " ");
  });
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(Container::Vertical({
      Instance("focus", focus),
      Instance("focusCursorBlock", focusCursorBlock),
      Instance("focusCursorBlockBlinking", focusCursorBlockBlinking),
      Instance("focusCursorBar", focusCursorBar),
      Instance("focusCursorBarBlinking", focusCursorBarBlinking),
      Instance("focusCursorUnderline", focusCursorUnderline),
      Instance("focusCursorUnderlineBlinking", focusCursorUnderlineBlinking),
  }));
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
