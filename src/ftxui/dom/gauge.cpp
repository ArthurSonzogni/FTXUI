#include <algorithm>  // for max, min
#include <memory>     // for allocator, make_shared
#include <string>     // for string

#include "ftxui/dom/elements.hpp"     // for Element, gauge
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Screen

namespace ftxui {

static std::string charset_right[11] = {
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

static std::string charset_left[7] = {" ", " ", " ", "▕", "▐", "█", "█"};

static std::string charset_up[11] = {" ", " ", "▁", "▂", "▃",
                                     "▄", "▅", "▆", "▇", "█"};

static std::string charset_down[7] = {" ", " ", " ", "▔", "▀", "█", "█"};

class Gauge : public Node {
 public:
  Gauge(float progress, GaugeDirection direction)
      : progress_(std::min(std::max(progress, 0.f), 1.f)),
        direction_(direction) {}

  void ComputeRequirement() override {
    switch (direction_) {
      case GaugeDirection::RIGHT:
      case GaugeDirection::LEFT:
        requirement_.flex_grow_x = 1;
        requirement_.flex_grow_y = 0;
        requirement_.flex_shrink_x = 1;
        requirement_.flex_shrink_y = 0;
        break;
      case GaugeDirection::UP:
      case GaugeDirection::DOWN:
        requirement_.flex_grow_x = 0;
        requirement_.flex_grow_y = 1;
        requirement_.flex_shrink_x = 0;
        requirement_.flex_shrink_y = 1;
        break;
    }
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    switch (direction_) {
      case GaugeDirection::RIGHT:
        RenderRight(screen);
        break;
      case GaugeDirection::UP:
        RenderUp(screen);
        break;
      case GaugeDirection::LEFT:
        RenderLeft(screen);
        break;
      case GaugeDirection::DOWN:
        RenderDown(screen);
        break;
    }
  }

  void RenderRight(Screen& screen) {
    int y = box_.y_min;
    if (y > box_.y_max)
      return;

    float limit = box_.x_min + progress_ * (box_.x_max - box_.x_min + 1);
    int limit_int = limit;
    int x = box_.x_min;
    while (x < limit_int)
      screen.at(x++, y) = charset_right[9];
    screen.at(x++, y) = charset_right[int(9 * (limit - limit_int))];
    while (x <= box_.x_max)
      screen.at(x++, y) = charset_right[0];
  }

  void RenderLeft(Screen& screen) {
    int y = box_.y_min;
    if (y > box_.y_max)
      return;

    float limit = box_.x_max - (progress_ * box_.x_max) + 1;
    int limit_int = limit;
    int x = box_.x_max;
    while (x > limit_int)
      screen.at(x--, y) = charset_left[6];
    screen.at(x++, y) = charset_left[int(6 - (6 * (limit - limit_int)))];
    while (x <= box_.x_min)
      screen.at(x--, y) = charset_left[0];
  }

  void RenderUp(Screen& screen) {
    int x = box_.x_min;
    if (x > box_.x_max)
      return;

    float limit = box_.y_max - (progress_ * box_.y_max) + 1;
    int limit_int = limit;

    int y = box_.y_max;
    while (y > limit_int)
      screen.at(x, y--) = charset_up[9];
    screen.at(x, y--) = charset_up[int(9 - (9 * (limit - limit_int)))];
    while (y >= box_.y_min)
      screen.at(x, y--) = charset_up[0];
  }

  void RenderDown(Screen& screen) {
    int x = box_.x_min;
    if (x > box_.x_max)
      return;

    float limit = box_.y_min + (progress_ * box_.y_max) + 1;
    int limit_int = limit;

    int y = box_.y_min;
    while (y < limit_int)
      screen.at(x, y++) = charset_down[6];
    screen.at(x, y++) = charset_down[int(6 * (limit - limit_int))];
    while (y <= box_.y_max)
      screen.at(x, y++) = charset_down[0];
  }

 private:
  float progress_;
  GaugeDirection direction_;
};

/// @brief Draw a high definition progress bar progressing in specified
/// direction.
/// @param progress The proportion of the area to be filled. Belong to [0,1].
//  @param direction Direction of progress bars progression.
/// @ingroup dom
Element gaugeDirection(float progress, GaugeDirection direction) {
  return std::make_shared<Gauge>(progress, direction);
}

/// @brief Draw a high definition progress bar progressing from left to right.
/// @param progress The proportion of the area to be filled. Belong to [0,1].
/// @ingroup dom
///
/// ### Example
///
/// A gauge. It can be used to represent a progress bar.
/// ~~~cpp
/// border(gaugeRight(0.5))
/// ~~~
///
/// #### Output
///
/// ~~~bash
/// ┌──────────────────────────────────────────────────────────────────────────┐
/// │█████████████████████████████████████                                     │
/// └──────────────────────────────────────────────────────────────────────────┘
/// ~~~
Element gaugeRight(float progress) {
  return gaugeDirection(progress, GaugeDirection::RIGHT);
}

/// @brief Draw a high definition progress bar progressing from right to left.
/// @param progress The proportion of the area to be filled. Belong to [0,1].
/// @ingroup dom
///
/// ### Example
///
/// A gauge. It can be used to represent a progress bar.
/// ~~~cpp
/// border(gaugeLeft(0.5))
/// ~~~
///
/// #### Output
///
/// ~~~bash
/// ┌──────────────────────────────────────────────────────────────────────────┐
/// │                                     █████████████████████████████████████│
/// └──────────────────────────────────────────────────────────────────────────┘
/// ~~~
Element gaugeLeft(float progress) {
  return gaugeDirection(progress, GaugeDirection::LEFT);
}

/// @brief Draw a high definition progress bar progressing from bottom to top.
/// @param progress The proportion of the area to be filled. Belong to [0,1].
/// @ingroup dom
///
/// ### Example
///
/// A gauge. It can be used to represent a progress bar.
/// ~~~cpp
/// border(gaugeUp(0.5))
/// ~~~
///
/// #### Output
///
/// ~~~bash
///  ┌─┐
///  │ │
///  │ │
///  │ │
///  │ │
///  │█│
///  │█│
///  │█│
///  │█│
///  └─┘
/// ~~~
Element gaugeUp(float progress) {
  return gaugeDirection(progress, GaugeDirection::UP);
}

/// @brief Draw a high definition progress bar progressing from top to bottom.
/// @param progress The proportion of the area to be filled. Belong to [0,1].
/// @ingroup dom
///
/// ### Example
///
/// A gauge. It can be used to represent a progress bar.
/// ~~~cpp
/// border(gaugeDown(0.5))
/// ~~~
///
/// #### Output
///
/// ~~~bash
///  ┌─┐
///  │█│
///  │█│
///  │█│
///  │█│
///  │ │
///  │ │
///  │ │
///  │ │
///  └─┘
/// ~~~
Element gaugeDown(float progress) {
  return gaugeDirection(progress, GaugeDirection::DOWN);
}

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
  return gaugeRight(progress);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
