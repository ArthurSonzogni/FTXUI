#include <stddef.h>                // for size_t
#include <ftxui/dom/elements.hpp>  // for operator|, size, Element, text, hcenter, Decorator, WIDTH, hflow, window, EQUAL, GREATER_THAN, HEIGHT, bold, border, dim, LESS_THAN
#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <ftxui/screen/string.hpp>  // for to_wstring
#include <memory>                   // for shared_ptr
#include <string>  // for allocator, operator+, char_traits, wstring

#include "ftxui/dom/node.hpp"    // for Render
#include "ftxui/screen/box.hpp"  // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto make_box = [](size_t dimx, size_t dimy) {
    std::wstring title = to_wstring(dimx) + L"x" + to_wstring(dimy);
    return window(text(title) | hcenter | bold,
                  text(L"content") | hcenter | dim) |
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

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
