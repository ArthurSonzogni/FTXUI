#include <functional>  // for function
#include <memory>      // for shared_ptr, __shared_ptr_access, allocator
#include <string>      // for string, basic_string
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Menu, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, color, separator, Decorator, bgcolor, flex, Element, bold, hbox, border, dim
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::BlueLight, Color::Red, Color::Yellow

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<std::string> entries = {
      "Monkey", "Dog", "Cat", "Bird", "Elephant",
  };
  int menu_1_selected_ = 0;
  int menu_2_selected_ = 0;
  int menu_3_selected_ = 0;
  int menu_4_selected_ = 0;
  int menu_5_selected_ = 0;
  int menu_6_selected_ = 0;

  MenuOption option_1;
  option_1.style_focused = bold | color(Color::Blue);
  option_1.style_selected = color(Color::Blue);
  option_1.style_selected_focused = bold | color(Color::Blue);
  option_1.on_enter = screen.ExitLoopClosure();
  auto menu_1_ = Menu(&entries, &menu_1_selected_, &option_1);

  MenuOption option_2;
  option_2.style_focused = bold | color(Color::Blue);
  option_2.style_selected = color(Color::Blue);
  option_2.style_selected_focused = bold | color(Color::Blue);
  option_2.on_enter = screen.ExitLoopClosure();
  auto menu_2_ = Menu(&entries, &menu_2_selected_, &option_2);

  MenuOption option_3;
  option_3.style_selected = color(Color::Blue);
  option_3.style_focused = bgcolor(Color::Blue);
  option_3.style_selected_focused = bgcolor(Color::Blue);
  option_3.on_enter = screen.ExitLoopClosure();
  auto menu_3_ = Menu(&entries, &menu_3_selected_, &option_3);

  MenuOption option_4;
  option_4.style_selected = bgcolor(Color::Blue);
  option_4.style_focused = bgcolor(Color::BlueLight);
  option_4.style_selected_focused = bgcolor(Color::BlueLight);
  option_4.on_enter = screen.ExitLoopClosure();
  auto menu_4_ = Menu(&entries, &menu_4_selected_, &option_4);

  MenuOption option_5;
  option_5.style_normal = bgcolor(Color::Blue);
  option_5.style_selected = bgcolor(Color::Yellow);
  option_5.style_focused = bgcolor(Color::Red);
  option_5.style_selected_focused = bgcolor(Color::Red);
  option_5.on_enter = screen.ExitLoopClosure();
  auto menu_5_ = Menu(&entries, &menu_5_selected_, &option_5);

  MenuOption option_6;
  option_6.style_normal = dim | color(Color::Blue);
  option_6.style_selected = color(Color::Blue);
  option_6.style_focused = bold | color(Color::Blue);
  option_6.style_selected_focused = bold | color(Color::Blue);
  option_6.on_enter = screen.ExitLoopClosure();
  auto menu_6_ = Menu(&entries, &menu_6_selected_, &option_6);

  auto container = Container::Horizontal({
      menu_1_,
      menu_2_,
      menu_3_,
      menu_4_,
      menu_5_,
      menu_6_,
  });

  // clang-format off
  auto renderer = Renderer(container, [&] {
   return
      hbox({
        menu_1_->Render() | flex, separator(),
        menu_2_->Render() | flex, separator(),
        menu_3_->Render() | flex, separator(),
        menu_4_->Render() | flex, separator(),
        menu_5_->Render() | flex, separator(),
        menu_6_->Render() | flex,
      }) | border;
  });
  // clang-format on

  screen.Loop(renderer);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
