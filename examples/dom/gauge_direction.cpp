// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <chrono>                  // for operator""s, chrono_literals
#include <ftxui/dom/elements.hpp>  // for filler, operator|, separator, text, border, Element, vbox, vtext, hbox, center, gaugeDown, gaugeLeft, gaugeRight, gaugeUp
#include <ftxui/screen/screen.hpp>  // for Screen
#include <iostream>                 // for cout, endl, ostream
#include <string>  // for allocator, operator+, operator<<, string, to_string
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

    auto gauge_up =  //
        hbox({
            vtext("gauge vertical"),
            separator(),
            gaugeUp(percentage),
        }) |
        border;

    auto gauge_down =  //
        hbox({
            vtext("gauge vertical"),
            separator(),
            gaugeDown(percentage),
        }) |
        border;

    auto gauge_right =  //
        vbox({
            text("gauge horizontal"),
            separator(),
            gaugeRight(percentage),
        }) |
        border;

    auto gauge_left =  //
        vbox({
            text("gauge horizontal"),
            separator(),
            gaugeLeft(percentage),
        }) |
        border;

    auto document = hbox({
        gauge_up,
        filler(),
        vbox({
            gauge_right,
            filler(),
            text(data_downloaded) | border | center,
            filler(),
            gauge_left,
        }),
        filler(),
        gauge_down,
    });

    auto screen = Screen(32, 16);
    Render(screen, document);
    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.01s);
  }
  std::cout << std::endl;
}
