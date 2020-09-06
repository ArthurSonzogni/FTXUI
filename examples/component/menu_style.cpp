#include <iostream>
#include <thread>

#include "ftxui/component/container.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container);

    for (Menu* menu : {&menu_1, &menu_2, &menu_3, &menu_4, &menu_5, &menu_6}) {
      container.Add(menu);
      menu->entries = {
          L"Monkey", L"Dog", L"Cat", L"Bird", L"Elephant",
      };
      menu->on_enter = [this]() { on_enter(); };
    }

    menu_2.selected_style = color(Color::Blue);
    menu_2.focused_style = bold | color(Color::Blue);

    menu_3.selected_style = color(Color::Blue);
    menu_3.focused_style = bgcolor(Color::Blue);

    menu_4.selected_style = bgcolor(Color::Blue);
    menu_4.focused_style = bgcolor(Color::BlueLight);

    menu_5.normal_style = bgcolor(Color::Blue);
    menu_5.selected_style = bgcolor(Color::Yellow);
    menu_5.focused_style = bgcolor(Color::Red);

    menu_6.normal_style = dim | color(Color::Blue);
    menu_6.selected_style = color(Color::Blue);
    menu_6.focused_style = bold | color(Color::Blue);
  }

  std::function<void()> on_enter = []() {};

 private:
  Container container = Container::Horizontal();
  Menu menu_1;
  Menu menu_2;
  Menu menu_3;
  Menu menu_4;
  Menu menu_5;
  Menu menu_6;

  // clang-format off
   Element Render() override {
     return
        hbox({
          menu_1.Render() | flex, separator(),
          menu_2.Render() | flex, separator(),
          menu_3.Render() | flex, separator(),
          menu_4.Render() | flex, separator(),
          menu_5.Render() | flex, separator(),
          menu_6.Render() | flex,
        }) | border;
   }
  // clang-format on
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop(&component);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
