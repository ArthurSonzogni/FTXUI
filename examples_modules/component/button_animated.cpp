#include <typeinfo>
#include <initializer_list>
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_string
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

int main() {
  int value = 50;

  // The tree of components. This defines how to navigate using the keyboard.
  auto buttons = Container::Horizontal({
      Button(
          "Decrease", [&] { value--; }, ButtonOption::Animated(Color::Red)),
      Button(
          "Reset", [&] { value = 50; }, ButtonOption::Animated(Color::Green)),
      Button(
          "Increase", [&] { value++; }, ButtonOption::Animated(Color::Blue)),
  });

  // Modify the way to render them on screen:
  auto component = Renderer(buttons, [&] {
    return vbox({
        vbox({
            text("value = " + std::to_string(value)),
            separator(),
            gauge(value * 0.01f),
        }) | border,
        buttons->Render(),
    });
  });

  auto screen = App::FitComponent();
  screen.Loop(component);
  return 0;
}
