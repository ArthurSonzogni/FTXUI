#include <stddef.h>                       // for size_t
#include <algorithm>                      // for max, min, sort, copy
#include <cmath>                          // for fmod, cos, sin, M_PI
#include <ftxui/dom/linear_gradient.hpp>  // for LinearGradient::Stop, LinearGradient
#include <memory>    // for allocator_traits<>::value_type, make_shared
#include <optional>  // for optional, operator!=, operator<
#include <utility>   // for move
#include <vector>    // for vector

#include "ftxui/dom/elements.hpp"  // for Element, Decorator, bgcolor, color
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/color.hpp"   // for Color, Color::Default, Color::Blue
#include "ftxui/screen/screen.hpp"  // for Pixel, Screen

namespace ftxui {
namespace {

struct LinearGradientNormalized {
  float angle = 0.f;
  std::vector<Color> colors;
  std::vector<float> positions;
};

// Convert a LinearGradient to a normalized version.
LinearGradientNormalized Normalize(LinearGradient gradient) {
  // Handle gradient of size 0.
  if (gradient.stops.size() == 0) {
    return LinearGradientNormalized{
        0.f, {Color::Default, Color::Default}, {0.f, 1.f}};
  }

  // Fill in the two extent, if needed.
  if (!gradient.stops.front().position) {
    gradient.stops.front().position = 0;
  }
  if (!gradient.stops.back().position) {
    gradient.stops.back().position = 1.f;
  }

  // Fill in the blank, by interpolating positions.
  size_t last_checkpoint = 0;
  for (size_t i = 1; i < gradient.stops.size(); ++i) {
    if (!gradient.stops[i].position) {
      continue;
    }

    if (i - last_checkpoint >= 2) {
      const float min = gradient.stops[i].position.value();
      const float max = gradient.stops[last_checkpoint].position.value();
      for (size_t j = last_checkpoint + 1; j < i; ++j) {
        gradient.stops[j].position =
            min + (max - min) * (j - last_checkpoint) / (i - last_checkpoint);
      }
    }

    last_checkpoint = i;
  }

  // Sort the stops by position.
  std::sort(
      gradient.stops.begin(), gradient.stops.end(),
      [](const auto& a, const auto& b) { return a.position < b.position; });

  // If we don't being with zero, add a stop at zero.
  if (gradient.stops.front().position != 0) {
    gradient.stops.insert(gradient.stops.begin(), {Color::Default, 0.f});
  }
  // If we don't end with one, add a stop at one.
  if (gradient.stops.back().position != 1) {
    gradient.stops.push_back({Color::Default, 0.f});
  }

  // Normalize the angle.
  LinearGradientNormalized normalized;
  normalized.angle = std::fmod(std::fmod(gradient.angle, 360.f) + 360.f, 360.f);
  for (auto& stop : gradient.stops) {
    normalized.colors.push_back(stop.color);
    normalized.positions.push_back(stop.position.value());
  }
  return normalized;
}

Color Interpolate(const LinearGradientNormalized& gradient, float t) {
  // Find the right color in the gradient's stops.
  for (size_t i = 0; i < gradient.positions.size() - 1; ++i) {
    if (t < gradient.positions[i] ||  //
        t > gradient.positions[i + 1]) {
      continue;
    }

    const float t0 = gradient.positions[i];
    const float t1 = gradient.positions[i + 1];
    const float tt = (t - t0) / (t1 - t0);

    const Color& c0 = gradient.colors[i];
    const Color& c1 = gradient.colors[i + 1];
    const Color& cc = Color::Interpolate(tt, c0, c1);

    return cc;
  }

  return Color::Blue;
}

class LinearGradientColor : public NodeDecorator {
 public:
  explicit LinearGradientColor(Element child,
                               const LinearGradient& gradient,
                               bool background_color)
      : NodeDecorator(std::move(child)),
        gradient_(Normalize(gradient)),
        background_color_{background_color} {}

  void Render(Screen& screen) override {
    const float dx = std::cos(gradient_.angle * M_PI / 180);
    const float dy = std::sin(gradient_.angle * M_PI / 180);

    // Project every corner to get the extent of the gradient.
    const float p1 = box_.x_min * dx + box_.y_min * dy;
    const float p2 = box_.x_min * dx + box_.y_max * dy;
    const float p3 = box_.x_max * dx + box_.y_min * dy;
    const float p4 = box_.x_max * dx + box_.y_max * dy;
    const float min = std::min({p1, p2, p3, p4});
    const float max = std::max({p1, p2, p3, p4});

    // Renormalize the projection to [0, 1] using the extent and projective
    // geometry.
    const float dX = dx / (max - min);
    const float dY = dy / (max - min);
    const float dZ = -min / (max - min);

    // Project every pixel to get the color.
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        const float t = x * dX + y * dY + dZ;
        GetColorRef(screen, x, y) = Interpolate(gradient_, t);
      }
    }

    NodeDecorator::Render(screen);
  }

  // Utility to get background or foreground color from pixel on screen
  Color& GetColorRef(Screen& screen, int x, int y) {
    return background_color_ ? screen.PixelAt(x, y).background_color
                             : screen.PixelAt(x, y).foreground_color;
  }

  LinearGradientNormalized gradient_;
  bool background_color_;
};

}  // namespace

/// @brief Set the foreground color of an element with linear-gradient effect.
/// @param gradient The gradient effect to be applied on the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// color(LinearGradient{0, {Color::Red, Color::Blue}}, text("Hello"))
/// ```
Element color(const LinearGradient& gradient, Element child) {
  return std::make_shared<LinearGradientColor>(std::move(child), gradient,
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
/// bgcolor(LinearGradient{0, {Color::Red, Color::Blue}}, text("Hello"))
/// ```
Element bgcolor(const LinearGradient& gradient, Element child) {
  return std::make_shared<LinearGradientColor>(std::move(child), gradient,
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
/// text("Hello") | color(LinearGradient{0, {Color::Red, Color::Blue}})
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
/// text("Hello") | color(LinearGradient{0, {Color::Red, Color::Blue}})
/// ```
Decorator bgcolor(const LinearGradient& gradient) {
  return
      [gradient](Element child) { return bgcolor(gradient, std::move(child)); };
}

}  // namespace ftxui

// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
