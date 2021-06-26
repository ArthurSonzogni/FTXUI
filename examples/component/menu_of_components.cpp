#include <functional>  // for function
#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <string>    // for wstring, allocator, basic_string
#include <vector>    // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/menu.hpp"                // for MenuBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<std::wstring> radiobox_list = {
      L"Use gcc",
      L"Use clang",
      L"Use emscripten",
      L"Use tcc",
  };
  int selected_rbox = 0;

  auto buttons = Radiobox(&radiobox_list, &selected_rbox);

  std::vector<std::wstring> toggle_1_entries = {
      L"On",
      L"Off",
  };
  std::vector<std::wstring> toggle_2_entries = {
      L"Enabled",
      L"Disabled",
  };
  std::vector<std::wstring> toggle_3_entries = {
      L"10€",
      L"0€",
  };
  std::vector<std::wstring> toggle_4_entries = {
      L"Nothing",
      L"One element",
      L"Several elements",
  };

  int toggle_1_selected = 0;
  int toggle_2_selected = 0;
  int toggle_3_selected = 0;
  int toggle_4_selected = 0;

  Component toggle_1 = Toggle(&toggle_1_entries, &toggle_1_selected);
  Component toggle_2 = Toggle(&toggle_2_entries, &toggle_2_selected);
  Component toggle_3 = Toggle(&toggle_3_entries, &toggle_3_selected);
  Component toggle_4 = Toggle(&toggle_4_entries, &toggle_4_selected);

  auto container = Container::Vertical({
      toggle_1,
      toggle_2,
      toggle_3,
      toggle_4,
  });

  std::vector<std::wstring> entries_ = {
      L"Monkey", L"Dog", L"Cat", L"Bird", L"Elephant",
  };
  int menu_1_selected_ = 0;
  int menu_2_selected_ = 0;
  int menu_3_selected_ = 0;
  int menu_4_selected_ = 0;
  int menu_5_selected_ = 0;
  int menu_6_selected_ = 0;
  auto menu_1_ = Menu(&entries_, &menu_1_selected_);
  auto menu_2_ = Menu(&entries_, &menu_2_selected_);
  auto menu_3_ = Menu(&entries_, &menu_3_selected_);
  auto menu_4_ = Menu(&entries_, &menu_4_selected_);
  auto menu_5_ = Menu(&entries_, &menu_5_selected_);
  auto menu_6_ = Menu(&entries_, &menu_6_selected_);
  auto container_2 = Container::Horizontal({
      menu_1_,
      menu_2_,
      menu_3_,
      menu_4_,
      menu_5_,
      menu_6_,
  });

  // clang-format off
  auto renderer = Renderer(container_2, [&] {
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

  for (Component menu : {menu_1_, menu_2_, menu_3_, menu_4_, menu_5_, menu_6_})
    MenuBase<std::vector<std::wstring>*>::From(menu)->on_enter =
        screen.ExitLoopClosure();

  MenuBase<std::vector<std::wstring>*>::From(menu_2_)->focused_style =
      bold | color(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_2_)->selected_style =
      color(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_2_)->selected_focused_style =
      bold | color(Color::Blue);

  MenuBase<std::vector<std::wstring>*>::From(menu_3_)->selected_style =
      color(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_3_)->focused_style =
      bgcolor(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_3_)->selected_focused_style =
      bgcolor(Color::Blue);

  MenuBase<std::vector<std::wstring>*>::From(menu_4_)->selected_style =
      bgcolor(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_4_)->focused_style =
      bgcolor(Color::BlueLight);
  MenuBase<std::vector<std::wstring>*>::From(menu_4_)->selected_focused_style =
      bgcolor(Color::BlueLight);

  MenuBase<std::vector<std::wstring>*>::From(menu_5_)->normal_style =
      bgcolor(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_5_)->selected_style =
      bgcolor(Color::Yellow);
  MenuBase<std::vector<std::wstring>*>::From(menu_5_)->focused_style =
      bgcolor(Color::Red);
  MenuBase<std::vector<std::wstring>*>::From(menu_5_)->selected_focused_style =
      bgcolor(Color::Red);

  MenuBase<std::vector<std::wstring>*>::From(menu_6_)->normal_style =
      dim | color(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_6_)->selected_style =
      color(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_6_)->focused_style =
      bold | color(Color::Blue);
  MenuBase<std::vector<std::wstring>*>::From(menu_6_)->selected_focused_style =
      bold | color(Color::Blue);

  Components entries = {
      buttons,
      container,
      renderer,
  };
  int selected = 0;

  auto menu = Menu<Components>(entries, &selected);

  MenuBase<Components>::From(menu)->selected_focused_style =
      color(Color::Chartreuse1);
  screen.Loop(menu);

  std::cout << "Selected Component = " << selected << std::endl;
  std::cout << "Selected rbox = " << selected_rbox << std::endl;
  std::cout << "Toggle 1 = " << toggle_1_selected << std::endl;
  std::cout << "Toggle 2 = " << toggle_2_selected << std::endl;
  std::cout << "Toggle 3 = " << toggle_3_selected << std::endl;
  std::cout << "Toggle 4 = " << toggle_4_selected << std::endl;
  std::cout << "Menu 1 = " << menu_1_selected_ << std::endl;
  std::cout << "Menu 2 = " << menu_2_selected_ << std::endl;
  std::cout << "Menu 3 = " << menu_3_selected_ << std::endl;
  std::cout << "Menu 4 = " << menu_4_selected_ << std::endl;
  std::cout << "Menu 5 = " << menu_5_selected_ << std::endl;
  std::cout << "Menu 6 = " << menu_6_selected_ << std::endl;
}

// Example by Shreyas Atre <shreyasatre16@gmail.com> github@SAtacker
// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
