// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stdio.h>  // for getchar
#include <ftxui/dom/elements.hpp>  // for Elements, gridbox, Fit, operator|, text, border, Element
#include <ftxui/screen/screen.hpp>  // for Screen
#include <memory>                   // for allocator, shared_ptr

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

int main() {
  using namespace ftxui;
  auto cell = [](const char* t) { return text(t) | border; };
  auto document =  //
      gridbox({
          {
              cell("north-west"),
              cell("north"),
              cell("north-east"),
          },
          {
              cell("center-west"),
              gridbox({
                  {
                      cell("center-north-west"),
                      cell("center-north-east"),
                  },
                  {
                      cell("center-south-west"),
                      cell("center-south-east"),
                  },
              }),
              cell("center-east"),
          },
          {
              cell("south-west"),
              cell("south"),
              cell("south-east"),
          },
      });
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  getchar();

  return 0;
}
