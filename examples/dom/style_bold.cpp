// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_BUILD_MODULES
#include <ftxui/dom/elements.hpp>  // for text, operator|, bold, Fit, hbox, Element
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#endif 

#include <memory>                   // for allocator

#ifndef FTXUI_BUILD_MODULES
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui
#else
import ftxui.dom;
import ftxui.screen;
#endif

int main() {
  using namespace ftxui;
  auto document =  //
      hbox({
          text("This text is "),
          text("bold") | bold,
          text(". Do you like it?"),
      });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}
