#include <ftxui/dom/elements.hpp>  // for operator|, Element, bgcolor, color, blink, bold, dim, inverted, underlined, hbox
#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <memory>                   // for allocator

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui
#include "ftxui/screen/color.hpp"    // for Color, Color::Blue

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    hbox({
      text(L"normal")                           , text(L" ") ,
      text(L"bold")      | bold                 , text(L" ") ,
      text(L"dim")       | dim                  , text(L" ") ,
      text(L"inverted")  | inverted             , text(L" ") ,
      text(L"underlined")| underlined           , text(L" ") ,
      text(L"blink")     | blink                , text(L" ") ,
      text(L"color")     | color(Color::Blue)   , text(L" ") ,
      text(L"bgcolor")   | bgcolor(Color::Blue),
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
