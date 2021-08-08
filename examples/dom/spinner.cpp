#include <chrono>                  // for operator""s, chrono_literals
#include <ftxui/dom/elements.hpp>  // for Element, operator|, separator, filler, hbox, size, spinner, vbox, bold, border, EQUAL, WIDTH
#include <ftxui/screen/screen.hpp>  // for Screen, Dimension
#include <ftxui/screen/string.hpp>  // for to_wstring
#include <iostream>                 // for cout, endl, ostream
#include <string>                   // for operator<<, string
#include <thread>                   // for sleep_for
#include <utility>                  // for move
#include <vector>                   // for vector

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  using namespace std::chrono_literals;

  std::string reset_position;
  for (int index = 0; index < 200; ++index) {
    std::vector<Element> entries;
    for (int i = 0; i < 22; ++i) {
      if (i != 0)
        entries.push_back(separator());
      entries.push_back(  //
          hbox({
              text(to_wstring(i)) | size(WIDTH, EQUAL, 2),
              separator(),
              spinner(i, index) | bold,
          }));
    }
    auto document = hbox({
        vbox(std::move(entries)) | border,
        filler(),
    });
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.1s);
  }
  std::cout << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
