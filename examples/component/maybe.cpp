#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for string, basic_string, allocator
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Checkbox, Maybe, Radiobox, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, border

using namespace ftxui;
Component Border(Component child) {
  return Renderer(child, [child] { return child->Render() | border; });
}

int main(int argc, const char* argv[]) {
  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };
  int menu_1_selected = 0;
  int menu_2_selected = 0;
  auto menu_1 = Radiobox(&entries, &menu_1_selected);
  auto menu_2 = Radiobox(&entries, &menu_2_selected);

  menu_1 = Border(menu_1);
  menu_2 = Border(menu_2);

  bool menu_1_show = false;
  bool menu_2_show = false;

  auto layout = Container::Vertical({
      Checkbox("Show menu_1", &menu_1_show),
      Maybe(menu_1, &menu_1_show),
      Checkbox("Show menu_2", &menu_2_show),
      Maybe(menu_2, &menu_2_show),
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(layout);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
