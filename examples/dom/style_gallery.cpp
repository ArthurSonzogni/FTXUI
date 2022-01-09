#include <ftxui/dom/elements.hpp>  // for text, operator|, Element, bgcolor, color, blink, bold, dim, inverted, underlined, Fit, hbox
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <memory>                   // for allocator

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    hbox({
      text("normal")                           , text(" ") ,
      text("bold")      | bold                 , text(" ") ,
      text("dim")       | dim                  , text(" ") ,
      text("inverted")  | inverted             , text(" ") ,
      text("underlined")| underlined           , text(" ") ,
      text("blink")     | blink                , text(" ") ,
      text("color")     | color(Color::Blue)   , text(" ") ,
      text("bgcolor")   | bgcolor(Color::Blue),
    });
  // clang-format on
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
