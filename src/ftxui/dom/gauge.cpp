#include <memory>  // for allocator, make_shared
#include <string>  // for string

#include "ftxui/dom/elements.hpp"  // for GaugeDirection, Element, GaugeDirection::Down, GaugeDirection::Left, GaugeDirection::Right, GaugeDirection::Up, gauge, gaugeDirection, gaugeDown, gaugeLeft, gaugeRight, gaugeUp
#include "ftxui/dom/node.hpp"      // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Screen, Pixel

namespace ftxui {

// NOLINTNEXTLINE
static const std::string charset_horizontal[11] = {
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

// NOLINTNEXTLINE
static const std::string charset_vertical[10] = {
    "█",
    "▇",
    "▆",
    "▅",
    "▄",
    "▃",
    "▂",
    "▁",
    " ",
    // An extra character in case when the fuzzer manage to have:
    // int(8 * (limit - limit_int) = 8
    " ",
};

class Gauge : public Node {
 public:
  Gauge(float progress, GaugeDirection direction)
      : progress_(progress), direction_(direction) {
    // This handle NAN correctly:
    if (!(progress_ > 0.F)) {
      progress_ = 0.F;
    }
    if (!(progress_ < 1.F)) {
      progress_ = 1.F;
    }
  }

  void ComputeRequirement() override {
    switch (direction_) {
      case GaugeDirection::Right:
      case GaugeDirection::Left:
        requirement_.flex_grow_x = 1;
        requirement_.flex_grow_y = 0;
        requirement_.flex_shrink_x = 1;
        requirement_.flex_shrink_y = 0;
        break;
      case GaugeDirection::Up:
      case GaugeDirection::Down:
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
      case GaugeDirection::Right:
        RenderHorizontal(screen, /*invert=*/false);
        break;
      case GaugeDirection::Up:
        RenderVertical(screen, /*invert=*/false);
        break;
      case GaugeDirection::Left:
        RenderHorizontal(screen, /*invert=*/true);
        break;
      case GaugeDirection::Down:
        RenderVertical(screen, /*invert=*/true);
        break;
    }
  }

  void RenderHorizontal(Screen& screen, bool invert) {
    int y = box_.y_min;
    if (y > box_.y_max) {
      return;
    }

    // Draw the progress bar horizontally.
    {
      float progress = invert ? 1.F - progress_ : progress_;
      float limit =
          (float)box_.x_min + progress * (float)(box_.x_max - box_.x_min + 1);
      int limit_int = (int)limit;
      int x = box_.x_min;
      while (x < limit_int) {
        screen.at(x++, y) = charset_horizontal[9];  // NOLINT
      }
      // NOLINTNEXTLINE
      screen.at(x++, y) = charset_horizontal[int(9 * (limit - limit_int))];
      while (x <= box_.x_max) {
        screen.at(x++, y) = charset_horizontal[0];
      }
    }

    if (invert) {
      for (int x = box_.x_min; x <= box_.x_max; x++) {
        screen.PixelAt(x, y).inverted ^= true;
      }
    }
  }

  void RenderVertical(Screen& screen, bool invert) {
    int x = box_.x_min;
    if (x > box_.x_max) {
      return;
    }

    // Draw the progress bar vertically:
    {
      float progress = invert ? progress_ : 1.F - progress_;
      float limit =
          (float)box_.y_min + progress * (float)(box_.y_max - box_.y_min + 1);
      int limit_int = (int)limit;
      int y = box_.y_min;
      while (y < limit_int) {
        screen.at(x, y++) = charset_vertical[8];  // NOLINT
      }
      // NOLINTNEXTLINE
      screen.at(x, y++) = charset_vertical[int(8 * (limit - limit_int))];
      while (y <= box_.y_max) {
        screen.at(x, y++) = charset_vertical[0];
      }
    }

    if (invert) {
      for (int y = box_.y_min; y <= box_.y_max; y++) {
        screen.PixelAt(x, y).inverted ^= true;
      }
    }
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
  return gaugeDirection(progress, GaugeDirection::Right);
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
  return gaugeDirection(progress, GaugeDirection::Left);
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
  return gaugeDirection(progress, GaugeDirection::Up);
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
  return gaugeDirection(progress, GaugeDirection::Down);
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
