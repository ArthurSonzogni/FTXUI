// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <chrono>                  // for operator""s, chrono_literals
#include <cmath>                   // for sin
#include <ftxui/dom/elements.hpp>  // for graph, operator|, separator, color, Element, vbox, flex, inverted, operator|=, Fit, hbox, size, border, GREATER_THAN, HEIGHT
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <functional>               // for ref, reference_wrapper
#include <iostream>                 // for cout, ostream
#include <memory>                   // for shared_ptr
#include <string>                   // for operator<<, string
#include <thread>                   // for sleep_for
#include <vector>                   // for vector

#include "ftxui/dom/node.hpp"  // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::BlueLight, Color::RedLight, Color::YellowLight, ftxui

class Graph {
 public:
  std::vector<int> operator()(int width, int height) const {
    std::vector<int> output(width);
    for (int i = 0; i < width; ++i) {
      float v = 0;
      v += 0.1f * sin((i + shift) * 0.1f);        // NOLINT
      v += 0.2f * sin((i + shift + 10) * 0.15f);  // NOLINT
      v += 0.1f * sin((i + shift) * 0.03f);       // NOLINT
      v *= height;                                // NOLINT
      v += 0.5f * height;                         // NOLINT
      output[i] = static_cast<int>(v);
    }
    return output;
  }
  int shift = 0;
};

std::vector<int> triangle(int width, int height) {
  std::vector<int> output(width);
  for (int i = 0; i < width; ++i) {
    output[i] = i % (height - 4) + 2;
  }
  return output;
}

int main() {
  using namespace ftxui;
  using namespace std::chrono_literals;

  Graph my_graph;

  std::string reset_position;
  for (int i = 0;; ++i) {
    auto document = hbox({
        vbox({
            graph(std::ref(my_graph)),
            separator(),
            graph(triangle) | inverted,
        }) | flex,
        separator(),
        vbox({
            graph(std::ref(my_graph)) | color(Color::BlueLight),
            separator(),
            graph(std::ref(my_graph)) | color(Color::RedLight),
            separator(),
            graph(std::ref(my_graph)) | color(Color::YellowLight),
        }) | flex,
    });

    document |= border;

    const int min_width = 40;
    document |= size(HEIGHT, GREATER_THAN, min_width);

    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();

    const auto sleep_time = 0.03s;
    std::this_thread::sleep_for(sleep_time);
    my_graph.shift++;
  }

  return 0;
}
