// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <memory>    // for shared_ptr, __shared_ptr_access
#include <string>    // for to_string, allocator

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for MenuEntryAnimated, Renderer, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for MenuEntryAnimated
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, separator, Element, Decorator, color, text, hbox, size, bold, frame, inverted, vbox, HEIGHT, LESS_THAN, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Cyan, Color::Green, Color::Red, Color::Yellow

using namespace ftxui;

// Define a special style for some menu entry.
MenuEntryOption Colored(ftxui::Color c) {
  MenuEntryOption option;
  option.animated_colors.foreground.enabled = true;
  option.animated_colors.background.enabled = true;
  option.animated_colors.background.active = c;
  option.animated_colors.background.inactive = Color::Black;
  option.animated_colors.foreground.active = Color::White;
  option.animated_colors.foreground.inactive = c;
  return option;
}

int main() {
  auto screen = ScreenInteractive::TerminalOutput();

  int selected = 0;
  auto menu = Container::Vertical(
      {
          MenuEntry(" 1. rear", Colored(Color::Red)),
          MenuEntry(" 2. drown", Colored(Color::Yellow)),
          MenuEntry(" 3. nail", Colored(Color::Green)),
          MenuEntry(" 4. quit", Colored(Color::Cyan)),
          MenuEntry(" 5. decorative", Colored(Color::Blue)),
          MenuEntry(" 7. costume"),
          MenuEntry(" 8. pick"),
          MenuEntry(" 9. oral"),
          MenuEntry("11. minister"),
          MenuEntry("12. football"),
          MenuEntry("13. welcome"),
          MenuEntry("14. copper"),
          MenuEntry("15. inhabitant"),
      },
      &selected);

  // Display together the menu with a border
  auto renderer = Renderer(menu, [&] {
    return vbox({
               hbox(text("selected = "), text(std::to_string(selected))),
               separator(),
               menu->Render() | frame,
           }) |
           border | bgcolor(Color::Black);
  });

  screen.Loop(renderer);

  std::cout << "Selected element = " << selected << std::endl;
}
