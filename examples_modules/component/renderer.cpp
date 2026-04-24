#include <typeinfo>
#include <initializer_list>
#include <memory>  // for shared_ptr, allocator, __shared_ptr_access
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;
  auto screen = App::FitComponent();

  // A Renderer() is a component using a lambda function as a parameter to
  // render itself.

  // 1. Example of focusable renderer:
  auto renderer_focusable = Renderer([](bool focused) {
    if (focused) {
      return text("FOCUSABLE RENDERER()") | center | bold | border;
    } else {
      return text(" Focusable renderer() ") | center | border;
    }
  });

  // 2. Examples of a non focusable renderer.
  auto renderer_non_focusable = Renderer([&] {
    return text("~~~~~ Non Focusable renderer() ~~~~~");  //
  });

  // 3. Renderer can wrap other components to redefine their Render() function.
  auto button = Button("Wrapped quit button", screen.ExitLoopClosure());
  auto renderer_wrap = Renderer(button, [&] {
    if (button->Focused()) {
      return button->Render() | bold | color(Color::Red);
    } else {
      return button->Render();
    }
  });

  // Let's renderer everyone:
  screen.Loop(Container::Vertical({
      renderer_focusable,
      renderer_non_focusable,
      renderer_wrap,
  }));
}
