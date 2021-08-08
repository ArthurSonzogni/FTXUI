#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_wstring

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, gauge, Element, operator|, vbox, border

using namespace ftxui;

int main(int argc, const char* argv[]) {
  int value = 50;

  // The tree of components. This defines how to navigate using the keyboard.
  auto button_option = ButtonOption();
  button_option.border = false;
  auto buttons = Container::Horizontal({
      Button(
          "[Decrease]", [&] { value--; }, &button_option),
      Button(
          "[Increase]", [&] { value++; }, &button_option),
  });

  // Modify the way to render them on screen:
  auto component = Renderer(buttons, [&] {
    return vbox({
               text("value = " + std::to_string(value)),
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
