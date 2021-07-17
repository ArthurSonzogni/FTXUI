#include <ftxui/dom/elements.hpp>  // for operator|, border, Element, vbox, center, dbox
#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <memory>                   // for allocator

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document = dbox({
      vbox({
          text(L"line_1"),
          text(L"line_2"),
          text(L"line_3"),
          text(L"line_4"),
          text(L"line_5"),
      }) | border,
      text(L"overlay") | border | center,
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
