#include <functional>        // for function
#include <initializer_list>  // for initializer_list
#include <memory>            // for __shared_ptr_access
#include <string>            // for wstring, allocator, basic_string
#include <vector>            // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Menu, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/menu.hpp"            // for MenuBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, Element, separator, bgcolor, color, flex, Decorator, bold, hbox, border, dim
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::BlueLight, Color::Red, Color::Yellow

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  std::vector<std::wstring> entries = {
      L"Monkey", L"Dog", L"Cat", L"Bird", L"Elephant",
  };
  int menu_1_selected_ = 0;
  int menu_2_selected_ = 0;
  int menu_3_selected_ = 0;
  int menu_4_selected_ = 0;
  int menu_5_selected_ = 0;
  int menu_6_selected_ = 0;
  auto menu_1_ = Menu(&entries, &menu_1_selected_);
  auto menu_2_ = Menu(&entries, &menu_2_selected_);
  auto menu_3_ = Menu(&entries, &menu_3_selected_);
  auto menu_4_ = Menu(&entries, &menu_4_selected_);
  auto menu_5_ = Menu(&entries, &menu_5_selected_);
  auto menu_6_ = Menu(&entries, &menu_6_selected_);
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
  auto screen = ScreenInteractive::TerminalOutput();
  for (Component menu : {menu_1_, menu_2_, menu_3_, menu_4_, menu_5_, menu_6_})
    MenuBase::From(menu)->on_enter = screen.ExitLoopClosure();

  MenuBase::From(menu_2_)->focused_style = bold | color(Color::Blue);
  MenuBase::From(menu_2_)->selected_style = color(Color::Blue);
  MenuBase::From(menu_2_)->selected_focused_style = bold | color(Color::Blue);

  MenuBase::From(menu_3_)->selected_style = color(Color::Blue);
  MenuBase::From(menu_3_)->focused_style = bgcolor(Color::Blue);
  MenuBase::From(menu_3_)->selected_focused_style = bgcolor(Color::Blue);

  MenuBase::From(menu_4_)->selected_style = bgcolor(Color::Blue);
  MenuBase::From(menu_4_)->focused_style = bgcolor(Color::BlueLight);
  MenuBase::From(menu_4_)->selected_focused_style = bgcolor(Color::BlueLight);

  MenuBase::From(menu_5_)->normal_style = bgcolor(Color::Blue);
  MenuBase::From(menu_5_)->selected_style = bgcolor(Color::Yellow);
  MenuBase::From(menu_5_)->focused_style = bgcolor(Color::Red);
  MenuBase::From(menu_5_)->selected_focused_style = bgcolor(Color::Red);

  MenuBase::From(menu_6_)->normal_style = dim | color(Color::Blue);
  MenuBase::From(menu_6_)->selected_style = color(Color::Blue);
  MenuBase::From(menu_6_)->focused_style = bold | color(Color::Blue);
  MenuBase::From(menu_6_)->selected_focused_style = bold | color(Color::Blue);

  screen.Loop(renderer);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
