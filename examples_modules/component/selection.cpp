#include <typeinfo>
#include <initializer_list>
#include <string>  // for char_traits, operator+, string, basic_string
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

Element LoremIpsum() {
  return text(
      "FTXUI: A powerful library for building user interfaces.\n"
      "Enjoy a rich set of components and a declarative style.\n"
      "Create beautiful and responsive UIs with minimal effort.\n"
      "Join the community and experience the power of FTXUI.");
}

int main() {
  auto screen = App::TerminalOutput();

  auto quit =
      Button("Quit", screen.ExitLoopClosure(), ButtonOption::Animated());

  int selection_change_counter = 0;
  std::string selection_content = "";
  screen.SelectionChange([&] {
    selection_change_counter++;
    selection_content = screen.GetSelection();
  });

  // The components:
  auto renderer = Renderer(quit, [&] {
    return vbox({
        text("Select changed: " + std::to_string(selection_change_counter) +
             " times"),
        text("Currently selected: "),
        paragraph(selection_content) | vscroll_indicator | frame | border |
            size(HEIGHT, EQUAL, 10),
        window(text("Horizontal split"), hbox({
                                             LoremIpsum(),
                                             separator(),
                                             LoremIpsum(),
                                             separator(),
                                             LoremIpsum(),
                                         })),
        window(text("Vertical split"), vbox({
                                           LoremIpsum(),
                                           separator(),
                                           LoremIpsum(),
                                           separator(),
                                           LoremIpsum(),
                                       })),
        window(text("Grid split with different style"),
               vbox({
                   hbox({
                       LoremIpsum(),
                       separator(),
                       LoremIpsum()                                   //
                           | selectionBackgroundColor(Color::Yellow)  //
                           | selectionColor(Color::Black)             //
                           | selectionStyleReset,
                       separator(),
                       LoremIpsum() | selectionColor(Color::Blue),
                   }),
                   separator(),
                   hbox({
                       LoremIpsum() | selectionColor(Color::Red),
                       separator(),
                       LoremIpsum() | selectionStyle([](Cell& pixel) {
                         pixel.underlined_double = true;
                       }),
                       separator(),
                       LoremIpsum(),
                   }),
               })),
        quit->Render(),
    });
  });

  screen.Loop(renderer);
}
