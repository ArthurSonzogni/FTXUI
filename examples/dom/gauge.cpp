// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <chrono>                  // for operator""s, chrono_literals
#include <ftxui/dom/elements.hpp>  // for text, gauge, operator|, flex, hbox, Element
#include <ftxui/screen/screen.hpp>  // for Screen
#include <iostream>                 // for cout, endl, ostream
#include <string>  // for allocator, char_traits, operator+, operator<<, string, to_string, basic_string
#include <thread>  // for sleep_for

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

int main() {
  using namespace ftxui;
  using namespace std::chrono_literals;

  std::string reset_position;
  for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.002f) {
    std::string data_downloaded =
        std::to_string(int(percentage * 5000)) + "/5000";
    auto document = hbox({
        text("downloading:"),
        gauge(percentage) | flex,
        text(" " + data_downloaded),
    });
    auto screen = Screen(100, 1);
    Render(screen, document);
    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.01s);
  }
  std::cout << std::endl;
}
