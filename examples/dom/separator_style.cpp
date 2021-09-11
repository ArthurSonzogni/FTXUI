#include <ftxui/dom/elements.hpp>  // for text, operator|, vbox, border, Element, Fit, hbox
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <iostream>
#include <memory>  // for allocator

#include "ftxui/dom/node.hpp"    // for Render
#include "ftxui/screen/box.hpp"  // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  auto document = vbox({
      vbox({
          text("separatorLight"),
          separatorLight(),
          hbox(text("left"), separatorLight(), text("right")),
      }) | borderLight,

      vbox({
          text("separatorHeavy"),
          separatorHeavy(),
          hbox(text("left"), separatorHeavy(), text("right")),
      }) | borderHeavy,

      vbox({
          text("separatorDouble"),
          separatorDouble(),
          hbox(text("left"), separatorDouble(), text("right")),
      }) | borderDouble,
  });

  auto screen =
      Screen::Create(Dimension::Fit(document), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  std::cout << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
