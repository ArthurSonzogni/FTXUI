#include <ftxui/dom/elements.hpp>  // for operator|, color, Element, bgcolor, graph, border
#include <ftxui/screen/screen.hpp>  // for Fixed, Screen
#include <vector>                   // for vector

#include "ftxui/dom/node.hpp"  // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::DarkBlue, Color::Green, Color::Red, ftxui

int main(void) {
  using namespace ftxui;
  Element document = graph([](int x, int y) {
                       std::vector<int> result(x, 0);
                       for (int i{0}; i < x; ++i) {
                         result[i] = ((3 * i) / 2) % y;
                       }
                       return result;
                     }) |
                     color(Color::Red) | border | color(Color::Green) |
                     bgcolor(Color::DarkBlue);

  auto screen = Screen::Create(Dimension::Fixed(80), Dimension::Fixed(10));
  Render(screen, document);
  screen.Print();
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
