// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for to_string, operator+

#ifndef FTXUI_BUILD_MODULES
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Button, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, text, Element, hbox, separator, size, vbox, border, frame, vscroll_indicator, HEIGHT, LESS_THAN
#include "ftxui/screen/color.hpp"  // for Color, Color::Default, Color::GrayDark, Color::White
#else
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
#endif

using namespace ftxui;

int main() {
  int counter = 0;
  auto on_click = [&] { counter++; };

  auto style = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                      Color::Default, Color::White);

  auto container = Container::Vertical({});
  for (int i = 0; i < 30; ++i) {
    auto button = Button("Button " + std::to_string(i), on_click, style);
    container->Add(button);
  }

  auto renderer = Renderer(container, [&] {
    return vbox({
               hbox({
                   text("Counter:"),
                   text(std::to_string(counter)),
               }),
               separator(),
               container->Render() | vscroll_indicator | frame |
                   size(HEIGHT, LESS_THAN, 20),
           }) |
           border;
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(renderer);

  return 0;
}
