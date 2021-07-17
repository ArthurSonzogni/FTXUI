#include <ftxui/dom/elements.hpp>  // for center, separator, operator|, flex, Element, vbox, hbox, border
#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <memory>                   // for allocator

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document = hbox({
                      text(L"left-column"),
                      separator(),
                      vbox({
                          center(text(L"right-top")) | flex,
                          separator(),
                          center(text(L"bottom-bottom")),
                      }) | flex,
                      separator(),
                      text(L"right-column"),
                  }) |
                  border;
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
