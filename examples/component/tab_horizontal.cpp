// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, operator|, vbox, border

using namespace ftxui;

int main() {
  std::vector<std::string> tab_values{
      "tab_1",
      "tab_2",
      "tab_3",
  };
  int tab_selected = 0;
  auto tab_toggle = Toggle(&tab_values, &tab_selected);

  std::vector<std::string> tab_1_entries{
      "Forest",
      "Water",
      "I don't know",
  };
  int tab_1_selected = 0;

  std::vector<std::string> tab_2_entries{
      "Hello",
      "Hi",
      "Hay",
  };
  int tab_2_selected = 0;

  std::vector<std::string> tab_3_entries{
      "Table",
      "Nothing",
      "Is",
      "Empty",
  };
  int tab_3_selected = 0;
  auto tab_container = Container::Tab(
      {
          Radiobox(&tab_1_entries, &tab_1_selected),
          Radiobox(&tab_2_entries, &tab_2_selected),
          Radiobox(&tab_3_entries, &tab_3_selected),
      },
      &tab_selected);

  auto container = Container::Vertical({
      tab_toggle,
      tab_container,
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
               tab_toggle->Render(),
               separator(),
               tab_container->Render(),
           }) |
           border;
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}
