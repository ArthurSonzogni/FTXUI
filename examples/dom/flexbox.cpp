#include <stddef.h>                // for size_t
#include <stdio.h>                 // for getchar
#include <ftxui/dom/elements.hpp>  // for operator|, size, Element, text, hcenter, Decorator, Fit, WIDTH, hflow, window, EQUAL, GREATER_THAN, HEIGHT, bold, border, dim, LESS_THAN
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <memory>                   // for allocator, shared_ptr
#include <string>  // for operator+, to_string, char_traits, string

#include "ftxui/dom/flexbox_config.hpp"  // for ftxui
#include "ftxui/dom/node.hpp"            // for Render

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  auto image = Canvas(100, 100);

  auto document = vbox({
    make_box("header"),
    hbox({
      make_box("left side"),
      make_box("center") | flex,
      make_box("right side"),
    }) | flex,
    make_box("footer")
  });

  //auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  //auto screen = Screen::Create(Dimension::Fit(document));
  auto screen = Screen::Create(Dimension::Full());
  Render(screen, document);
  screen.Print();
  getchar();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
