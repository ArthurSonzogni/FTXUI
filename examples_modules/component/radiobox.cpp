#include <typeinfo>
#include <initializer_list>
#include <string>  // for string, allocator, basic_string
#include <vector>  // for vector
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

int main() {
  std::vector<std::string> radiobox_list = {
      "Use gcc",
      "Use clang",
      "Use emscripten",
      "Use tcc",
  };
  int selected = 0;

  auto screen = App::TerminalOutput();
  screen.Loop(Radiobox(&radiobox_list, &selected));
  return 0;
}
