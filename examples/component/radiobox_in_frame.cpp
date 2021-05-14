#include <memory>  // for __shared_ptr_access, shared_ptr
#include <string>  // for wstring, operator+
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Radiobox, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, size, border, frame, HEIGHT, LESS_THAN
#include "ftxui/screen/string.hpp"  // for to_wstring

using namespace ftxui;

int main(int argc, const char* argv[]) {
  std::vector<std::wstring> entries;
  int selected = 0;

  for (int i = 0; i < 30; ++i)
    entries.push_back(L"RadioBox " + to_wstring(i));
  auto radiobox = Radiobox(&entries, &selected);
  auto renderer = Renderer(radiobox, [&] {
    return radiobox->Render() | frame | size(HEIGHT, LESS_THAN, 10) | border;
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(renderer);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
