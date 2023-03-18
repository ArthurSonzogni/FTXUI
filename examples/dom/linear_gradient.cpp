#include <ftxui/dom/elements.hpp>  // for bgcolor, operator|, operator|=, text, center, Element
#include <ftxui/dom/linear_gradient.hpp>  // for LinearGradient::Stop, LinearGradient
#include <ftxui/screen/screen.hpp>        // for Full, Screen
#include <memory>                         // for allocator, shared_ptr

#include "ftxui/dom/node.hpp"  // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::DeepPink1, Color::DeepSkyBlue1, Color::Yellow, ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document = text("gradient") | center;

  document |= bgcolor(LinearGradient{45,
                                     {
                                         {Color::DeepPink1},
                                         {Color::DeepSkyBlue1},
                                         {Color::Yellow},
                                         {Color::DeepPink1},
                                     }});
  auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
  Render(screen, document);
  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
