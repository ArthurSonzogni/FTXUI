#include <algorithm>  // for max, min
#include <memory>     // for allocator, make_shared
#include <string>     // for string

#include "ftxui/dom/elements.hpp"     // for Element, gauge
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Screen

namespace ftxui {

static std::string charset[11] = {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    // Microsoft's terminals often use fonts not handling the 8 unicode
    // characters for representing the whole gauge. Fallback with less.
    " ", " ", " ", " ", "▌", "▌", "▌", "█", "█", "█",
#else
    " ", " ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█",
#endif
    // An extra character in case when the fuzzer manage to have:
    // int(9 * (limit - limit_int) = 9
    "█"};

class Gauge : public Node {
 public:
  Gauge(float progress) : progress_(std::min(std::max(progress, 0.f), 1.f)) {}

  void ComputeRequirement() override {
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 0;
    requirement_.flex_shrink_x = 1;
    requirement_.flex_shrink_y = 0;
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    int y = box_.y_min;
    if (y > box_.y_max)
      return;

    float limit = box_.x_min + progress_ * (box_.x_max - box_.x_min + 1);
    int limit_int = limit;
    int x = box_.x_min;
    while (x < limit_int)
      screen.at(x++, y) = charset[9];
    screen.at(x++, y) = charset[int(9 * (limit - limit_int))];
    while (x <= box_.x_max)
      screen.at(x++, y) = charset[0];
  }

 private:
  float progress_;
};

/// @brief Draw a high definition progress bar.
/// @param progress The proportion of the area to be filled. Belong to [0,1].
/// @ingroup dom
///
/// ### Example
///
/// A gauge. It can be used to represent a progress bar.
/// ~~~cpp
/// border(gauge(0.5))
/// ~~~
///
/// #### Output
///
/// ~~~bash
/// ┌──────────────────────────────────────────────────────────────────────────┐
/// │█████████████████████████████████████                                     │
/// └──────────────────────────────────────────────────────────────────────────┘
/// ~~~
Element gauge(float progress) {
  return std::make_shared<Gauge>(progress);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
