#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <memory>    // for shared_ptr, __shared_ptr_access
#include <string>    // for to_string, allocator

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for MenuEntry, Renderer, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for MenuEntryOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, separator, Element, Decorator, color, text, hbox, size, bold, frame, inverted, vbox, HEIGHT, LESS_THAN, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Cyan, Color::Green, Color::Red, Color::Yellow

using namespace ftxui;

// Define a special style for some menu entry.
MenuEntryOption Colored(ftxui::Color c) {
  MenuEntryOption special_style;
  special_style.style_normal = Decorator(color(c));
  special_style.style_focused = Decorator(color(c)) | inverted;
  special_style.style_selected = Decorator(color(c)) | bold;
  special_style.style_selected_focused = Decorator(color(c)) | inverted | bold;
  return special_style;
}

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();

  int selected = 0;
  auto menu = Container::Vertical(
      {
          MenuEntry(" 1. improve"),
          MenuEntry(" 2. tolerant"),
          MenuEntry(" 3. career"),
          MenuEntry(" 4. cast"),
          MenuEntry(" 5. question"),

          Renderer([] { return separator(); }),

          MenuEntry(" 6. rear", Colored(Color::Red)),
          MenuEntry(" 7. drown", Colored(Color::Yellow)),
          MenuEntry(" 8. nail", Colored(Color::Green)),
          MenuEntry(" 9. quit", Colored(Color::Cyan)),
          MenuEntry("10. decorative", Colored(Color::Blue)),

          Renderer([] { return separator(); }),

          MenuEntry("11. costume"),
          MenuEntry("12. pick"),
          MenuEntry("13. oral"),
          MenuEntry("14. minister"),
          MenuEntry("15. football"),
          MenuEntry("16. welcome"),
          MenuEntry("17. copper"),
          MenuEntry("18. inhabitant"),
          MenuEntry("19. fortune"),
      },
      &selected);

  // Display together the menu with a border
  auto renderer = Renderer(menu, [&] {
    return vbox({
               hbox(text("selected = "), text(std::to_string(selected))),
               separator(),
               menu->Render() | frame | size(HEIGHT, LESS_THAN, 10),
           }) |
           border;
  });

  screen.Loop(renderer);

  std::cout << "Selected element = " << selected << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
