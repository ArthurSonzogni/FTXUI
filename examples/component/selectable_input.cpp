// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for char_traits, operator+, string, basic_string

#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/util/ref.hpp"  // for Ref

using namespace ftxui;

Element LoremIpsum() {
  return vbox({
      text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
           "eiusmod tempor incididunt ut labore et dolore magna aliqua."),
      text("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
           "nisi ut aliquip ex ea commodo consequat."),
      text("Duis aute irure dolor in reprehenderit in voluptate velit esse "
           "cillum dolore eu fugiat nulla pariatur."),
      text("Excepteur sint occaecat cupidatat non proident, sunt in culpa qui "
           "officia deserunt mollit anim id est laborum."),
  });
}

int main() {
  auto screen = ScreenInteractive::TerminalOutput();

  auto quit = Button("Quit", screen.ExitLoopClosure());

  // The components:
  auto renderer = Renderer(quit, [&] {
    return vbox({
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
        window(text("Vertical split"),
               vbox({
                   window(text("horizontal split"), hbox({
                                                        LoremIpsum(),
                                                        separator(),
                                                        LoremIpsum(),
                                                        separator(),
                                                        LoremIpsum(),
                                                    })),
                   separator(),
                   window(text("horizontal split"), hbox({
                                                        LoremIpsum(),
                                                        separator(),
                                                        LoremIpsum(),
                                                        separator(),
                                                        LoremIpsum(),
                                                    })),
               })),
        quit->Render(),
    });
  });

  screen.Loop(renderer);
}
