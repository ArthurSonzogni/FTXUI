#include <algorithm>  // for reverse
#include <cassert>    // for assert
#include <memory>     // for make_shared
#include <utility>    // for move, ref

#include "ftxui/dom/elements.hpp"  // for Element, Decorator, bgcolor, color
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/color.hpp"        // for Color
#include "ftxui/screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

// LinearGradient constructor with a default direction
LinearGradient::LinearGradient(const Colors& colors)
    : direction_{Direction::Right}, colors_{colors} {}

// LinearGradient constructor
LinearGradient::LinearGradient(LinearGradient::Direction direction,
                               const Colors& colors)
    : direction_{direction}, colors_{colors} {}

/*
  This replicate the CSS linear-gradient effect.
  See guide for documentation:
  https://css-tricks.com/css3-gradients/
*/

class GradientColor : public NodeDecorator {
 public:
  explicit GradientColor(Element child,
                         const LinearGradient& config,
                         bool background_color)
      : NodeDecorator(std::move(child)),
        config_{config},
        background_color_{background_color} {
    // TODO: I'm not sure what should be the default behaviour for an invalid
    // config in an element/decorator, so I've used 'assert'...
    assert(config_.colors().size() >= 2);

    // In this case, decorator should apply a reverse linear-gradient effect
    if (config.direction() == LinearGradient::Direction::Left ||
        config.direction() == LinearGradient::Direction::Up) {
      Colors& colors = config_.colors();
      std::reverse(colors.begin(), colors.end());
    }
  }

  void Render(Screen& screen) override {
    switch (config_.direction()) {
      case LinearGradient::Direction::Left:
      case LinearGradient::Direction::Right:
        RenderHorizontal(screen);
        break;
      case LinearGradient::Direction::Down:
      case LinearGradient::Direction::Up:
        RenderVertical(screen);
        break;
    }
  }

  void RenderHorizontal(Screen& screen) {
    Colors& colors = config_.colors();

    // Calculate total width and pixel length for each gradient color
    const int width = box_.x_max - box_.x_min + 1;
    const int region_per_pair = width / ((int)colors.size() - 1);

    Colors::const_iterator first, second;

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        float ratio;

        // Simply interpolate first and last color
        if (width == 1) {
          ratio = 0.5F;
          first = colors.begin();
          second = std::prev(colors.end());
        } else {
          // Calculate region to choose which pair of colors to decorate
          int index = x - box_.x_min;
          int region = index / region_per_pair;

          // Interpolation ratio
          ratio = float(index % region_per_pair) / float(region_per_pair - 1);

          // Get pair of colors to interpolate
          first = colors.begin() + region;
          second = std::next(first);
        }

        // Apply color style to pixel from decorated element
        Color& pixel_color = GetColorRef(screen, x, y);
        pixel_color = Color::Interpolate(ratio, *first, *second);
      }
    }

    NodeDecorator::Render(screen);
  }

  void RenderVertical(Screen& screen) {
    Colors& colors = config_.colors();

    // Calculate total width and pixel length for each gradient color
    const int height = box_.y_max - box_.y_min + 1;
    const int region_per_pair = height / ((int)colors.size() - 1);

    Colors::const_iterator first, second;

    for (int x = box_.x_min; x <= box_.x_max; ++x) {
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        float ratio;

        // Simply interpolate first and last color
        if (height == 1) {
          ratio = 0.5F;
          first = colors.begin();
          second = std::prev(colors.end());
        } else {
          // Calculate region to choose which pair of colors to decorate
          int index = y - box_.y_min;
          int region = index / region_per_pair;

          // Interpolation ratio
          ratio = float(index % region_per_pair) / float(region_per_pair - 1);

          // Get pair of colors to interpolate
          first = colors.begin() + region;
          second = std::next(first);
        }

        // Apply color style to pixel from decorated element
        Color& pixel_color = GetColorRef(screen, x, y);
        pixel_color = Color::Interpolate(ratio, *first, *second);
      }
    }

    NodeDecorator::Render(screen);
  }

  // Utility to get background or foreground color from pixel on screen
  Color& GetColorRef(Screen& screen, int x, int y) {
    return background_color_ ? std::ref(screen.PixelAt(x, y).background_color)
                             : std::ref(screen.PixelAt(x, y).foreground_color);
  }

  LinearGradient config_;
  bool background_color_;
};

/// @brief Set the foreground color of an element with linear-gradient effect.
/// @param gradient The gradient effect to be applied on the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = color(LinearGradient({Color::GreenLight, Color::Green}),
/// text("Success")),
/// ```
Element color(const LinearGradient& gradient, Element child) {
  return std::make_shared<GradientColor>(std::move(child), gradient,
                                         /*background_color*/ false);
}

/// @brief Set the background color of an element with linear-gradient effect.
/// @param gradient The gradient effect to be applied on the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = bgcolor(LinearGradient({Color::GreenLight,
/// Color::Green}), text("Success")),
/// ```
Element bgcolor(const LinearGradient& gradient, Element child) {
  return std::make_shared<GradientColor>(std::move(child), gradient,
                                         /*background_color*/ true);
}

/// @brief Decorate using a linear-gradient effect on the foreground color.
/// @param gradient The gradient effect to be applied on the output element.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | color(LinearGradient(Color::RedLight,
/// Color::Red));
/// ```
Decorator color(const LinearGradient& gradient) {
  return
      [gradient](Element child) { return color(gradient, std::move(child)); };
}

/// @brief Decorate using a linear-gradient effect on the background color.
/// @param gradient The gradient effect to be applied on the output element.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | bgcolor(LinearGradient(Color::RedLight,
/// Color::Red));
/// ```
Decorator bgcolor(const LinearGradient& gradient) {
  return
      [gradient](Element child) { return bgcolor(gradient, std::move(child)); };
}

}  // namespace ftxui

// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
