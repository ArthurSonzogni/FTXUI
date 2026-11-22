// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_LINEAR_GRADIENT_HPP
#define FTXUI_DOM_LINEAR_GRADIENT_HPP

#include <optional>
#include <vector>

#include "ftxui/screen/color.hpp"  // for Colors
#include "ftxui/util/export.hpp"   // for FTXUI_EXPORT

namespace ftxui {

/// @brief A class representing the settings for linear-gradient color effect.
///
/// Example:
/// ```cpp
/// LinearGradient()
///    .Angle(45)
///    .Stop(Color::Red, 0.0)
///    .Stop(Color::Green, 0.5)
///    .Stop(Color::Blue, 1.0);
/// ```
///
/// There are also shorthand constructors:
/// ```cpp
/// LinearGradient(Color::Red, Color::Blue);
/// LinearGradient(45, Color::Red, Color::Blue);
/// ```
///
/// @ingroup dom
struct FTXUI_EXPORT(DOM) LinearGradient {
  float angle = 0.f;

  /// A stop is a color at a specific position in the gradient.
  /// The position is a value between 0.0 and 1.0,
  /// where 0.0 is the start of the gradient
  /// and 1.0 is the end of the gradient.
  struct Stop {
    Color color = Color::Default;
    std::optional<float> position;
  };
  std::vector<Stop> stops;

  // Constructors:

  /// Create an empty linear gradient.
  LinearGradient();
  /// Create a horizontal gradient from `begin` to `end`.
  LinearGradient(Color begin, Color end);
  /// Create a gradient at `angle` degrees from `begin` to `end`.
  LinearGradient(float angle, Color begin, Color end);

  // Builder-pattern modifiers:

  /// Set the gradient angle in degrees.
  LinearGradient& Angle(float angle);
  /// Add a color stop at an explicit normalized `position` in [0, 1].
  LinearGradient& Stop(Color color, float position);
  /// Add a color stop with an automatically interpolated position.
  LinearGradient& Stop(Color color);
};

}  // namespace ftxui

#endif  // FTXUI_DOM_LINEAR_GRADIENT_HPP
