#include <stddef.h>                // for size_t
#include <stdio.h>                 // for getchar
#include <ftxui/dom/elements.hpp>  // for operator|, size, Element, text, hcenter, Decorator, Fit, WIDTH, hflow, window, EQUAL, GREATER_THAN, HEIGHT, bold, border, dim, LESS_THAN
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <memory>                   // for allocator, shared_ptr
#include <string>  // for operator+, to_string, char_traits, string

#include "ftxui/dom/flexbox_config.hpp"  // for ftxui
#include "ftxui/dom/node.hpp"            // for Render

#include <cmath>
int main(int argc, const char* argv[]) {
  using namespace ftxui;

  auto canvas = Canvas(100, 100);

  canvas.DrawText(0, 0, "This is a canvas", [](Pixel& p) -> void {
    p.foreground_color = Color::Red;
    p.underlined = true;
  });

  // Triangle:
  canvas.DrawPointLine(10, 10, 80, 10, Color::Red);
  canvas.DrawPointLine(80, 10, 80, 40, Color::Blue);
  canvas.DrawPointLine(80, 40, 10, 10, Color::Green);

  // Circle, not filled and filled:
  canvas.DrawPointCircle(30, 50, 20);
  canvas.DrawPointCircleFilled(40, 40, 10);

  // Plot a function:
  std::vector<int> ys(100);
  for (int x = 0; x < 100; x++)
    ys[x] = 80 + 20 * cos(x * 0.2);
  for (int x = 0; x < 99; x++) 
    canvas.DrawPointLine(x, ys[x], x + 1, ys[x + 1], Color::Red);

  auto document = ElementFrom(&canvas) | border;

  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  getchar();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
