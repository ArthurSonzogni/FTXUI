#include <stdlib.h>  // for EXIT_SUCCESS
#include <memory>    // for __shared_ptr_access
#include <string>  // for wstring, allocator, operator+, basic_string, char_traits
#include <vector>  // for vector, __alloc_traits<>::value_type

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Menu, Renderer, Horizontal, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, Element, operator|, window, flex, vbox
#include "ftxui/screen/string.hpp"  // for to_wstring

using namespace ftxui;

Component Window(std::wstring title, Component component) {
  return Renderer(component, [component, title] {  //
    return window(text(title), component->Render()) | flex;
  });
}

int main(int argc, const char* argv[]) {
  int menu_selected[] = {0, 0, 0};
  std::vector<std::vector<std::wstring>> menu_entries = {
      {
          L"Ananas",
          L"Raspberry",
          L"Citrus",
      },
      {
          L"Potatoes",
          L"Weat",
          L"Rise",
      },
      {
          L"Carrot",
          L"Lettuce",
          L"Tomato",
      },
  };

  int menu_selected_global = 0;
  auto menu_global = Container::Vertical(
      {
          Window(L"Menu 1", Menu(&menu_entries[0], &menu_selected[0])),
          Window(L"Menu 2", Menu(&menu_entries[1], &menu_selected[1])),
          Window(L"Menu 3", Menu(&menu_entries[2], &menu_selected[2])),
      },
      &menu_selected_global);

  auto info = Renderer([&] {
    int g = menu_selected_global;
    std::wstring value = menu_entries[g][menu_selected[g]];
    return window(text(L"Content"),  //
                  vbox({
                      text(L"menu_selected_global = " + to_wstring(g)),
                      text(L"menu_selected[0]     = " +
                           to_wstring(menu_selected[0])),
                      text(L"menu_selected[1]     = " +
                           to_wstring(menu_selected[1])),
                      text(L"menu_selected[2]     = " +
                           to_wstring(menu_selected[2])),
                      text(L"Value                = " + value),
                  })) |
           flex;
  });

  auto global = Container::Horizontal({
      menu_global,
      info,
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(global);
  return EXIT_SUCCESS;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
