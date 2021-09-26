#include <functional>  // for function
#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <string>    // for string, basic_string, allocator
#include <vector>    // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/component_options.hpp"   // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  std::vector<std::string> entries = {
      "tribute",     "clearance", "ally",     "bend",  "electronics",
      "module",      "era",       "cultural", "sniff", "nationalism",
      "negotiation", "deliver",   "figure",   "east",
      "tribute",     "clearance", "ally",     "bend",  "electronics",
      "module",      "era",       "cultural", "sniff", "nationalism",
      "negotiation", "deliver",   "figure",   "east",
      "tribute",     "clearance", "ally",     "bend",  "electronics",
      "module",      "era",       "cultural", "sniff", "nationalism",
      "negotiation", "deliver",   "figure",   "east",
  };

  int selected_1 = 0;
  int selected_2 = 0;
  int selected_3 = 0;
  int selected_4 = 0;

  auto layout = Container::Vertical({
      Container::Horizontal({
          Dropdown(&entries, &selected_1),
          Dropdown(&entries, &selected_2),
      }),
      Container::Horizontal({
          Dropdown(&entries, &selected_3),
          Dropdown(&entries, &selected_4),
      }),
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(layout);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
