#include <functional>  // for function
#include <iostream>    // for basic_ostream::ope...
#include <string>      // for wstring, allocator
#include <vector>      // for vector

#include "ftxui/component/menu.hpp"                // for Menu
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/screen/box.hpp"                    // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  Menu menu;
  menu.entries = {L"entry 1", L"entry 2", L"entry 3"};
  menu.selected = 0;
  menu.on_enter = screen.ExitLoopClosure();

  screen.Loop(&menu);

  std::cout << "Selected element = " << menu.selected << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
