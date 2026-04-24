#include <typeinfo>
#include <initializer_list>
#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for string, basic_string, operator+, to_string
#include <vector>  // for vector
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;

  Component input_list = Container::Vertical({});
  std::vector<std::string> items(100, "");
  for (size_t i = 0; i < items.size(); ++i) {
    input_list->Add(Input(&(items[i]), "placeholder " + std::to_string(i)));
  }

  auto renderer = Renderer(input_list, [&] {
    return input_list->Render() | vscroll_indicator | frame | border |
           size(HEIGHT, LESS_THAN, 10);
  });

  auto screen = App::TerminalOutput();
  screen.Loop(renderer);
}
