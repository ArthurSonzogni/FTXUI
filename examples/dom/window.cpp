#include <stdlib.h>                // for EXIT_SUCCESS
#include <ftxui/dom/elements.hpp>  // for operator|=, Element, bgcolor, color, graph, border
#include <ftxui/screen/screen.hpp>  // for Fixed, Screen
#include <vector>                   // for vector

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::DarkBlue, Color::Red, ftxui

int main() {
  using namespace ftxui;
  Element document = graph([](int x, int y) {
    std::vector<int> result(x, 0);
    for (int i{0}; i < x; ++i) {
      result[i] = ((3 * i) / 2) % y;
    }
    return result;
  });

  document |= color(Color::Red);
  document |= bgcolor(Color::DarkBlue);
  document |= border;

  const int width = 80;
  const int height = 10;
  auto screen =
      Screen::Create(Dimension::Fixed(width), Dimension::Fixed(height));
  Render(screen, document);
  screen.Print();
  return EXIT_SUCCESS;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
