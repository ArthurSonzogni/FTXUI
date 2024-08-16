// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <array>    // for array, array<>::value_type
#include <memory>   // for make_shared, allocator
#include <string>   // for basic_string, string
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"  // for Element, BorderStyle, LIGHT, separator, DOUBLE, EMPTY, HEAVY, separatorCharacter, separatorDouble, separatorEmpty, separatorHSelector, separatorHeavy, separatorLight, separatorStyled, separatorVSelector
#include "ftxui/dom/node.hpp"      // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/color.hpp"     // for Color
#include "ftxui/screen/pixel.hpp"     // for Pixel
#include "ftxui/screen/screen.hpp"    // for Pixel, Screen

namespace ftxui {

namespace {
using Charset = std::array<std::string, 2>;  // NOLINT
using Charsets = std::array<Charset, 6>;     // NOLINT
// NOLINTNEXTLINE
const Charsets charsets = {
    Charset{"│", "─"},  // LIGHT
    Charset{"╏", "╍"},  // DASHED
    Charset{"┃", "━"},  // HEAVY
    Charset{"║", "═"},  // DOUBLE
    Charset{"│", "─"},  // ROUNDED
    Charset{" ", " "},  // EMPTY
};

class Separator : public Node {
 public:
  explicit Separator(std::string value) : value_(std::move(value)) {}

  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        Pixel& pixel = screen.PixelAt(x, y);
        pixel.character = value_;
        pixel.automerge = true;
      }
    }
  }

  std::string value_;
};

class SeparatorAuto : public Node {
 public:
  explicit SeparatorAuto(BorderStyle style) : style_(style) {}

  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    const bool is_column = (box_.x_max == box_.x_min);
    const bool is_line = (box_.y_min == box_.y_max);

    const std::string c =
        charsets[style_][int(is_line && !is_column)];  // NOLINT

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        Pixel& pixel = screen.PixelAt(x, y);
        pixel.character = c;
        pixel.automerge = true;
      }
    }
  }

  BorderStyle style_;
};

class SeparatorWithPixel : public SeparatorAuto {
 public:
  explicit SeparatorWithPixel(Pixel pixel)
      : SeparatorAuto(LIGHT), pixel_(std::move(pixel)) {
    pixel_.automerge = true;
  }
  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y) = pixel_;
      }
    }
  }

 private:
  Pixel pixel_;
};
}  // namespace

/// @brief Draw a vertical or horizontal separation in between two other
/// elements.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separator(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ────
/// down
/// ```
Element separator() {
  return std::make_shared<SeparatorAuto>(LIGHT);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements.
/// @param style the style of the separator.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separatorStyled(DOUBLE),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ════
/// down
/// ```
Element separatorStyled(BorderStyle style) {
  return std::make_shared<SeparatorAuto>(style);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements, using the LIGHT style.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separatorLight(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ────
/// down
/// ```
Element separatorLight() {
  return std::make_shared<SeparatorAuto>(LIGHT);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements, using the DASHED style.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separatorLight(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ╍╍╍╍
/// down
/// ```
Element separatorDashed() {
  return std::make_shared<SeparatorAuto>(DASHED);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements, using the HEAVY style.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separatorHeavy(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ━━━━
/// down
/// ```
Element separatorHeavy() {
  return std::make_shared<SeparatorAuto>(HEAVY);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements, using the DOUBLE style.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separatorDouble(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ════
/// down
/// ```
Element separatorDouble() {
  return std::make_shared<SeparatorAuto>(DOUBLE);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements, using the EMPTY style.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separator(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
///
/// down
/// ```
Element separatorEmpty() {
  return std::make_shared<SeparatorAuto>(EMPTY);
}

/// @brief Draw a vertical or horizontal separation in between two other
/// elements.
/// @param value the character to fill the separator area.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorDouble
/// @see separatorHeavy
/// @see separatorEmpty
/// @see separatorRounded
/// @see separatorStyled
/// @see separatorCharacter
///
/// Add a visual separation in between two elements.
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = vbox({
///   text("up"),
///   separator(),
///   text("down"),
/// });
/// ```
///
/// ### Output
///
/// ```bash
/// up
/// ────
/// down
/// ```
Element separatorCharacter(std::string value) {
  return std::make_shared<Separator>(std::move(value));
}

/// @brief Draw a separator in between two element filled with a given pixel.
/// @ingroup dom
/// @see separator
/// @see separatorLight
/// @see separatorDashed
/// @see separatorHeavy
/// @see separatorDouble
/// @see separatorStyled
///
/// ### Example
///
/// ```cpp
/// Pixel empty;
/// Element document = vbox({
///   text("Up"),
///   separator(empty),
///   text("Down"),
/// })
/// ```
///
/// ### Output
///
/// ```bash
/// Up
///
/// Down
/// ```
Element separator(Pixel pixel) {
  return std::make_shared<SeparatorWithPixel>(std::move(pixel));
}

/// @brief Draw an horizontal bar, with the area in between left/right colored
/// differently.
/// @param left the left limit of the active area.
/// @param right the right limit of the active area.
/// @param selected_color the color of the selected area.
/// @param unselected_color the color of the unselected area.
///
/// ### Example
///
/// ```cpp
/// Element document = separatorHSelector(2,5, Color::White, Color::Blue);
/// ```
Element separatorHSelector(float left,
                           float right,
                           Color unselected_color,
                           Color selected_color) {
  class Impl : public Node {
   public:
    Impl(float left, float right, Color selected_color, Color unselected_color)
        : left_(left),
          right_(right),
          unselected_color_(unselected_color),
          selected_color_(selected_color) {}
    void ComputeRequirement() override {
      requirement_.min_x = 1;
      requirement_.min_y = 1;
    }

    void Render(Screen& screen) override {
      if (box_.y_max < box_.y_min) {
        return;
      }

      // This are the two location with an empty demi-cell.
      int demi_cell_left = int(left_ * 2.F - 1.F);    // NOLINT
      int demi_cell_right = int(right_ * 2.F + 2.F);  // NOLINT

      const int y = box_.y_min;
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        Pixel& pixel = screen.PixelAt(x, y);

        const int a = (x - box_.x_min) * 2;
        const int b = a + 1;
        const bool a_empty = demi_cell_left == a || demi_cell_right == a;
        const bool b_empty = demi_cell_left == b || demi_cell_right == b;

        if (!a_empty && !b_empty) {
          pixel.character = "─";
          pixel.automerge = true;
        } else {
          pixel.character = a_empty ? "╶" : "╴";  // NOLINT
          pixel.automerge = false;
        }

        if (demi_cell_left <= a && b <= demi_cell_right) {
          pixel.foreground_color = selected_color_;
        } else {
          pixel.foreground_color = unselected_color_;
        }
      }
    }

    float left_;
    float right_;
    Color unselected_color_;
    Color selected_color_;
  };
  return std::make_shared<Impl>(left, right, unselected_color, selected_color);
}

/// @brief Draw an vertical bar, with the area in between up/downcolored
/// differently.
/// @param up the left limit of the active area.
/// @param down the right limit of the active area.
/// @param selected_color the color of the selected area.
/// @param unselected_color the color of the unselected area.
///
/// ### Example
///
/// ```cpp
/// Element document = separatorHSelector(2,5, Color::White, Color::Blue);
/// ```
Element separatorVSelector(float up,
                           float down,
                           Color unselected_color,
                           Color selected_color) {
  class Impl : public Node {
   public:
    Impl(float up, float down, Color unselected_color, Color selected_color)
        : up_(up),
          down_(down),
          unselected_color_(unselected_color),
          selected_color_(selected_color) {}
    void ComputeRequirement() override {
      requirement_.min_x = 1;
      requirement_.min_y = 1;
    }

    void Render(Screen& screen) override {
      if (box_.x_max < box_.x_min) {
        return;
      }

      // This are the two location with an empty demi-cell.
      const int demi_cell_up = int(up_ * 2 - 1);
      const int demi_cell_down = int(down_ * 2 + 2);

      const int x = box_.x_min;
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        Pixel& pixel = screen.PixelAt(x, y);

        const int a = (y - box_.y_min) * 2;
        const int b = a + 1;
        const bool a_empty = demi_cell_up == a || demi_cell_down == a;
        const bool b_empty = demi_cell_up == b || demi_cell_down == b;

        if (!a_empty && !b_empty) {
          pixel.character = "│";
          pixel.automerge = true;
        } else {
          pixel.character = a_empty ? "╷" : "╵";  // NOLINT
          pixel.automerge = false;
        }

        if (demi_cell_up <= a && b <= demi_cell_down) {
          pixel.foreground_color = selected_color_;
        } else {
          pixel.foreground_color = unselected_color_;
        }
      }
    }

    float up_;
    float down_;
    Color unselected_color_;
    Color selected_color_;
  };
  return std::make_shared<Impl>(up, down, unselected_color, selected_color);
}

}  // namespace ftxui
