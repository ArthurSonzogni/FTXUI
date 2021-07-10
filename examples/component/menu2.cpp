#include <functional>  // for function
#include <memory>      // for allocator, __shared_ptr_access
#include <string>      // for wstring, operator+, to_string, basic_string
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Menu, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, separator, bold, hcenter, vbox, hbox, gauge, Element, operator|, border
#include "ftxui/screen/string.hpp"  // for to_wstring

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<std::wstring> left_menu_entries = {
      L"0%",  L"10%", L"20%", L"30%", L"40%",
      L"50%", L"60%", L"70%", L"80%", L"90%",
  };
  std::vector<std::wstring> right_menu_entries = {
      L"0%", L"1%", L"2%", L"3%", L"4%",  L"5%",
      L"6%", L"7%", L"8%", L"9%", L"10%",
  };

  auto menu_option = MenuOption();
  menu_option.on_enter = screen.ExitLoopClosure();

  int left_menu_selected = 0;
  int right_menu_selected = 0;
  Component left_menu_ =
      Menu(&left_menu_entries, &left_menu_selected, &menu_option);
  Component right_menu_ =
      Menu(&right_menu_entries, &right_menu_selected, &menu_option);

  Component container = Container::Horizontal({
      left_menu_,
      right_menu_,
  });

  auto renderer = Renderer(container, [&] {
    int sum = left_menu_selected * 10 + right_menu_selected;
    return vbox({
               // -------- Top panel --------------
               hbox({
                   // -------- Left Menu --------------
                   vbox({
                       hcenter(bold(text(L"Percentage by 10%"))),
                       separator(),
                       left_menu_->Render(),
                   }),
                   separator(),
                   // -------- Right Menu --------------
                   vbox({
                       hcenter(bold(text(L"Percentage by 1%"))),
                       separator(),
                       right_menu_->Render(),
                   }),
                   separator(),
               }),
               separator(),
               // -------- Bottom panel --------------
               vbox({
                   hbox({
                       text(L" gauge : "),
                       gauge(sum / 100.0),
                   }),
                   hbox({
                       text(L"  text : "),
                       text(to_wstring(std::to_string(sum) + " %")),
                   }),
               }),
           }) |
           border;
  });

  screen.Loop(renderer);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
