#include <typeinfo>
#include <initializer_list>
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for string, basic_string, operator+, to_string
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
  std::vector<std::string> entries;
  int selected = 0;

  for (int i = 0; i < 100; ++i) {
    entries.push_back(std::to_string(i));
  }
  auto radiobox = Menu(&entries, &selected, MenuOption::Horizontal());
  auto renderer = Renderer(
      radiobox, [&] { return radiobox->Render() | hscroll_indicator | frame; });

  auto screen = App::FitComponent();
  screen.Loop(renderer);

  return 0;
}
