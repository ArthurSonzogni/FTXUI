#include <memory>  // for __shared_ptr_access, shared_ptr
#include <string>  // for operator+, to_wstring

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, Element, gauge, text, operator|, vbox, border

using namespace ftxui;

int main(int argc, const char* argv[]) {
  int value = 50;

  // The tree of components. This defines how to navigate using the keyboard.
  auto buttons = Container::Horizontal({
      Button("[Decrease]", [&] { value--; }, false),
      Button("[Increase]", [&] { value++; }, false),
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
