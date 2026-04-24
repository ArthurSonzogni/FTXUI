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
  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };
  int menu_1_selected = 0;
  int menu_2_selected = 0;

  bool menu_1_show = false;
  bool menu_2_show = false;

  auto layout = Container::Vertical({
      Checkbox("Show menu_1", &menu_1_show),
      Radiobox(&entries, &menu_1_selected) | border | Maybe(&menu_1_show),
      Checkbox("Show menu_2", &menu_2_show),
      Radiobox(&entries, &menu_2_selected) | border | Maybe(&menu_2_show),

      Renderer([] {
        return text("You found the secret combination!") | color(Color::Red);
      }) | Maybe([&] { return menu_1_selected == 1 && menu_2_selected == 2; }),
  });

  auto screen = App::TerminalOutput();
  screen.Loop(layout);
}
