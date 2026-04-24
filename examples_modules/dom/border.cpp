#include <typeinfo>
#include <initializer_list>
#include <memory>                   // for allocator
#include <stdlib.h>                // for EXIT_SUCCESS
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;
  auto document =  //
      hbox({
          vbox({
              text("Line 1"),
              text("Line 2"),
              text("Line 3"),
          }) | border,

          vbox({
              text("Line 4"),
              text("Line 5"),
              text("Line 6"),
          }) | border,

          vbox({
              text("Line 7"),
              text("Line 8"),
              text("Line 9"),
          }) | border,
      });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  return EXIT_SUCCESS;
}
