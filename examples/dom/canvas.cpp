// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stdio.h>                 // for getchar
#include <cmath>                   // for cos
#include <ftxui/dom/elements.hpp>  // for Fit, canvas, operator|, border, Element
#include <ftxui/screen/screen.hpp>  // for Pixel, Screen
#include <vector>                   // for vector, allocator

#include "ftxui/dom/canvas.hpp"  // for Canvas
#include "ftxui/dom/node.hpp"    // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::Red, Color::Blue, Color::Green, ftxui

int main() {
  using namespace ftxui;

  auto c = Canvas(100, 100);

  c.DrawText(0, 0, "This is a canvas", [](Pixel& p) {
    p.foreground_color = Color::Red;
    p.underlined = true;
  });

  // Triangle:
  c.DrawPointLine(10, 10, 80, 10, Color::Red);
  c.DrawPointLine(80, 10, 80, 40, Color::Blue);
  c.DrawPointLine(80, 40, 10, 10, Color::Green);

  // Circle, not filled and filled:
  c.DrawPointCircle(30, 50, 20);
  c.DrawPointCircleFilled(40, 40, 10);

  // Plot a function:
  std::vector<int> ys(100);
  for (int x = 0; x < 100; x++)
    ys[x] = int(80 + 20 * cos(x * 0.2));
  for (int x = 0; x < 99; x++)
    c.DrawPointLine(x, ys[x], x + 1, ys[x + 1], Color::Red);

  auto document = canvas(&c) | border;

  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  getchar();

  return 0;
}
