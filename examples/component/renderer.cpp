#include <memory>  // for shared_ptr, allocator, __shared_ptr_access

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Renderer, Button, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/deprecated.hpp"                // for text
#include "ftxui/dom/elements.hpp"  // for operator|, Element, bold, border, center, color
#include "ftxui/screen/color.hpp"  // for Color, Color::Red

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::FitComponent();

  // A Renderer() is a component using a lambda function as a parameter to
  // render itself.

  // 1. Example of focusable renderer:
  auto renderer_focusable = Renderer([](bool focused) {
    if (focused)
      return text(L"FOCUSABLE RENDERER()") | center | bold | border;
    else
      return text(L" Focusable renderer() ") | center | border;
  });

  // 2. Examples of a non focusable renderer.
  auto renderer_non_focusable = Renderer([&] {
    return text(L"~~~~~ Non Focusable renderer() ~~~~~");  //
  });

  // 3. Renderer can wrap other components to redefine their Render() function.
  auto button = Button(L"Wrapped quit button", screen.ExitLoopClosure());
  auto renderer_wrap = Renderer(button, [&] {
    if (button->Focused())
      return button->Render() | bold | color(Color::Red);
    else
      return button->Render();
  });

  // Let's renderer everyone:
  screen.Loop(Container::Vertical({
      renderer_focusable,
      renderer_non_focusable,
      renderer_wrap,
  }));
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
