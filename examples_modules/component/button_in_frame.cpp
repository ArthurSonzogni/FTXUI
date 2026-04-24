#include <typeinfo>
#include <initializer_list>
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for to_string, operator+
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


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

  auto screen = App::FitComponent();
  screen.Loop(renderer);

  return 0;
}
