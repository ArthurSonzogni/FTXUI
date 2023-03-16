#ifndef FTXUI_DOM_LINEAR_GRADIENT_HPP
#define FTXUI_DOM_LINEAR_GRADIENT_HPP

#include <vector>

#include "ftxui/screen/color.hpp"  // for Colors

namespace ftxui {

/// @brief A class representing the settings for linear-gradient color effect.
class LinearGradient {
 public:
  enum class Direction;

  /// Constructors
  LinearGradient() = default;
  LinearGradient(const Colors&);  // Default direction.
  LinearGradient(Direction direction, const Colors& colors);

  /// This establishes the main-axis, thus defining the direction that colors
  /// are applied on the element.
  enum class Direction {
    Left,   ///< Colors are applied in a row, but in reverse order.
    Up,     ///< Colors are applied in a column.
    Right,  ///< Colors are applied in a row.
    Down    ///< Colors are applied in a column, but in reverse order.
  };

 private:
  friend class GradientColor;

  /// Getters
  Direction direction() const { return direction_; }
  Colors& colors() { return colors_; }
  bool empty() const { return colors_.empty(); }

  /// This defines in which direction to apply the colors on the element.
  Direction direction_;

  /// This defines the colors to be applied on the element.
  Colors colors_;
};

}  // namespace ftxui

#endif  // FTXUI_DOM_LINEAR_GRADIENT_HPP

// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
