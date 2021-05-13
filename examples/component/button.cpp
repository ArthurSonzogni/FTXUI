#include <string>  // for operator+, to_wstring, allocator, wstring

#include "ftxui/component/component.hpp"           // for Button, Make
#include "ftxui/component/container.hpp"           // for Container
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

using namespace ftxui;

int main(int argc, const char* argv[]) {
  int value = 50;
  std::wstring label_dec = L"decrease";
  std::wstring label_inc = L"increase";

  // The tree of components. This defines how to navigate using the keyboard.
  auto buttons = Container::Horizontal({
      Button(&label_dec, [&] { value--; }),
      Button(&label_inc, [&] { value++; }),
  });

  // Modify the way to render them on screen:
  auto component = Renderer(buttons, [&] {
    return vbox({
               text(L"value = " + std::to_wstring(value)),
               separator(),
               gauge(value * 0.01f),
               separator(),
               buttons->Render(),
           }) |
           border;
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(component);
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
