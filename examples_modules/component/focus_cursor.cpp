#include <typeinfo>
#include <initializer_list>
#include <string>  // for allocator, operator+, char_traits, string
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


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

int main() {
  auto screen = App::Fullscreen();
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
