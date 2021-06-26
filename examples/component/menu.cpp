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

  std::vector<std::wstring> entries = {
      L"entry 1",
      L"entry 2",
      L"entry 3",
  };
  int selected = 0;

  auto menu = Menu(&entries, &selected);
  MenuBase<std::vector<std::wstring>*>::From(menu)->on_enter =
      screen.ExitLoopClosure();

  screen.Loop(menu);

  std::cout << "Selected element = " << selected << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
