#ifndef FTXUI_DOM_LINEAR_GRADIENT_HPP
#define FTXUI_DOM_LINEAR_GRADIENT_HPP

#include <optional>
#include <vector>

#include "ftxui/screen/color.hpp"  // for Colors

namespace ftxui {

/// @brief A class representing the settings for linear-gradient color effect.
///
/// Example in with C++20 designated initializers
/// ```cpp
/// LinearGradient{
///   .angle = 0,
///   .stops = {
///      {Color::Red},
///      {Color::Green},
///      {Color::Blue},
///   });
/// ```
///
/// Example with C++17
/// ```cpp
/// LinearGradient{0.f, {Color::Red, Color::Green, Color::Blue}};
/// ```
struct LinearGradient {
  float angle = 0.f;
  struct Stop {
    Color color = Color::Default;
    std::optional<float> position;
  };
  std::vector<Stop> stops;

  // Simple constructor
  LinearGradient();
  LinearGradient(Color begin, Color end);
  LinearGradient(float angle, Color begin, Color end);

  // Modifier using the builder pattern.
  LinearGradient& Angle(float angle);
  LinearGradient& Stop(Color color, float position);
  LinearGradient& Stop(Color color);
};

}  // namespace ftxui

#endif  // FTXUI_DOM_LINEAR_GRADIENT_HPP

// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
