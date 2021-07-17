#include <ftxui/dom/elements.hpp>  // for operator|, vbox, border, Element, hbox
#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <memory>                   // for allocator

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document =  //
      hbox({
          vbox({
              text(L"Line 1"),
              text(L"Line 2"),
              text(L"Line 3"),
          }) | border,

          vbox({
              text(L"Line 4"),
              text(L"Line 5"),
              text(L"Line 6"),
          }) | border,

          vbox({
              text(L"Line 7"),
              text(L"Line 8"),
              text(L"Line 9"),
          }) | border,
      });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
