#include <memory>  // for __shared_ptr_access
#include <string>  // for wstring, allocator, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Radiobox, Make, Toggle
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, operator|, vbox, border

using namespace ftxui;

int main(int argc, const char* argv[]) {
  std::vector<std::wstring> tab_values{
      L"tab_1",
      L"tab_2",
      L"tab_3",
  };
  int tab_selected = 0;
  auto tab_toggle = Toggle(&tab_values, &tab_selected);

  std::vector<std::wstring> tab_1_entries{
      L"Forest",
      L"Water",
      L"I don't know",
  };
  int tab_1_selected = 0;

  std::vector<std::wstring> tab_2_entries{
      L"Hello",
      L"Hi",
      L"Hay",
  };
  int tab_2_selected = 0;

  std::vector<std::wstring> tab_3_entries{
      L"Table",
      L"Nothing",
      L"Is",
      L"Empty",
  };
  int tab_3_selected = 0;
  auto tab_container = Container::Tab(
      &tab_selected, {
                         Radiobox(&tab_1_entries, &tab_1_selected),
                         Radiobox(&tab_2_entries, &tab_2_selected),
                         Radiobox(&tab_3_entries, &tab_3_selected),
                     });

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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
