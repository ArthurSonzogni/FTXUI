// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stdio.h>                 // for getchar
#include <ftxui/dom/elements.hpp>  // for operator|, size, Element, text, hcenter, Decorator, Fit, WIDTH, hflow, window, EQUAL, GREATER_THAN, HEIGHT, bold, border, dim, LESS_THAN
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <string>  // for allocator, char_traits, operator+, to_string, string

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

int main() {
  using namespace ftxui;
  auto make_box = [](int dimx, int dimy) {
    std::string title = std::to_string(dimx) + "x" + std::to_string(dimy);
    return window(text(title) | hcenter | bold,
                  text("content") | hcenter | dim) |
           size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
  };

  auto style = size(WIDTH, GREATER_THAN, 20) | border |
               size(HEIGHT, GREATER_THAN, 30) | size(WIDTH, LESS_THAN, 50);

  auto document = hflow({
                      make_box(7, 7),
                      make_box(7, 5),
                      make_box(5, 7),
                      make_box(10, 4),
                      make_box(10, 4),
                      make_box(10, 4),
                      make_box(10, 4),
                      make_box(11, 4),
                      make_box(11, 4),
                      make_box(11, 4),
                      make_box(11, 4),
                      make_box(12, 4),
                      make_box(12, 5),
                      make_box(12, 4),
                      make_box(13, 4),
                      make_box(13, 3),
                      make_box(13, 3),
                      make_box(10, 3),
                  }) |
                  style;

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  getchar();

  return 0;
}
