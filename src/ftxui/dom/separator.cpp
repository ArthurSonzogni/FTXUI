#include <memory>  // for make_shared
#include <string>  // for string

#include "ftxui/dom/elements.hpp"     // for Element, separator
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Pixel, Screen

namespace ftxui {

using ftxui::Screen;

const std::string charset[][2] = {
    {"│", "─"},  //
    {"┃", "━"},  //
    {"║", "═"},  //
    {"│", "─"},  //
    {" ", " "},  //
};

class Separator : public Node {
 public:
  Separator(std::string value) : value_(value) {}

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
  SeparatorAuto(BorderStyle style) : style_(style) {}

  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    bool is_column = (box_.x_max == box_.x_min);
    bool is_line = (box_.y_min == box_.y_max);

    const std::string c = charset[style_][is_line && !is_column];

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
  SeparatorWithPixel(Pixel pixel) : SeparatorAuto(LIGHT), pixel_(pixel) {
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

/// @brief Draw a vertical or horizontal separation in between two other
/// elements.
/// @ingroup dom
/// @see separator
/// @see separatorLight
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
/// elements, using the HEAVY style.
/// @ingroup dom
/// @see separator
/// @see separatorLight
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
  return std::make_shared<Separator>(value);
}

/// @brief Draw a separator in between two element filled with a given pixel.
/// @ingroup dom
/// @see separator
/// @see separatorLight
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
  return std::make_shared<SeparatorWithPixel>(pixel);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
