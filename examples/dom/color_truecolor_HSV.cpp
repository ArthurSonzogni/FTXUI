#include <ftxui/dom/elements.hpp>  // for operator|, Elements, bgcolor, color, hbox, vbox, Element
#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <memory>                   // for allocator
#include <utility>                  // for move

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui
#include "ftxui/screen/color.hpp"    // for Color

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  int saturation = 255;
  Elements array;
  for (int value = 0; value < 255; value += 20) {
    Elements line;
    for (int hue = 0; hue < 255; hue += 2) {
      line.push_back(text(L"▀")                                   //
                     | color(Color::HSV(hue, saturation, value))  //
                     | bgcolor(Color::HSV(hue, saturation, value + 10)));
    }
    array.push_back(hbox(std::move(line)));
  }

  auto document = vbox(std::move(array));

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
