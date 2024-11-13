// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for min
#include <memory>     // for make_shared
#include <string>     // for string, wstring
#include <utility>    // for move

#include "ftxui/dom/deprecated.hpp"   // for text, vtext
#include "ftxui/dom/elements.hpp"     // for Element, text, vtext
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Pixel, Screen
#include "ftxui/screen/string.hpp"  // for string_width, Utf8ToGlyphs, to_string

namespace ftxui {

namespace {
using ftxui::Screen;

class Text : public Node {
 public:
  explicit Text(std::string text) : text_(std::move(text)) {}

  void ComputeRequirement() override {
    requirement_.min_x = string_width(text_);
    requirement_.min_y = 1;
  }

  void Selection(Box selection, std::vector<Box>* selected) override {
    if (Box::Intersection(selection, box_).IsEmpty()) {
      return;
    }

    const bool xmin_satured =
        selection.y_min < box_.y_min || selection.x_min < box_.x_min;
    const bool xmax_satured =
        selection.y_max > box_.y_max || selection.x_max > box_.x_max;

    selection_start_ = xmin_satured ? box_.x_min : selection.x_min;
    selection_end_ = xmax_satured ? box_.x_max : selection.x_max;

    has_selection = true;

    Box out;
    out.x_min = selection_start_;
    out.x_max = selection_end_;
    out.y_min = box_.y_min;
    out.y_max = box_.y_max;
    selected->push_back(out);
  }
  
  void Render(Screen& screen) override {
    int x = box_.x_min;
    const int y = box_.y_min;
    if (y > box_.y_max) {
      return;
    }

    for (const auto& cell : Utf8ToGlyphs(text_)) {
      if (x > box_.x_max) {
        break;
      }
      if (cell == "\n") {
        continue;
      }
      screen.PixelAt(x, y).character = cell;

      ++x;
    }

    if (!has_selection) {
      return;
    }

    // Invert the selection
    for(int x = selection_start_; x <= selection_end_; x++) {
      screen.PixelAt(x, y).inverted = true;
    }
  }

 private:
  std::string text_;
  bool has_selection = false;
  int selection_start_ = 0;
  int selection_end_ = 10;
};

class VText : public Node {
 public:
  explicit VText(std::string text)
      : text_(std::move(text)), width_{std::min(string_width(text_), 1)} {}

  void ComputeRequirement() override {
    requirement_.min_x = width_;
    requirement_.min_y = string_width(text_);
  }

  void Render(Screen& screen) override {
    const int x = box_.x_min;
    int y = box_.y_min;
    if (x + width_ - 1 > box_.x_max) {
      return;
    }
    for (const auto& it : Utf8ToGlyphs(text_)) {
      if (y > box_.y_max) {
        return;
      }
      screen.PixelAt(x, y).character = it;
      y += 1;
    }
  }

 private:
  std::string text_;
  int width_ = 1;
};

}  // namespace

/// @brief Display a piece of UTF8 encoded unicode text.
/// @ingroup dom
/// @see ftxui::to_wstring
///
/// ### Example
///
/// ```cpp
/// Element document = text("Hello world!");
/// ```
///
/// ### Output
///
/// ```bash
/// Hello world!
/// ```
Element text(std::string text) {
  return std::make_shared<Text>(std::move(text));
}

/// @brief Display a piece of unicode text.
/// @ingroup dom
/// @see ftxui::to_wstring
///
/// ### Example
///
/// ```cpp
/// Element document = text(L"Hello world!");
/// ```
///
/// ### Output
///
/// ```bash
/// Hello world!
/// ```
Element text(std::wstring text) {  // NOLINT
  return std::make_shared<Text>(to_string(text));
}

/// @brief Display a piece of unicode text vertically.
/// @ingroup dom
/// @see ftxui::to_wstring
///
/// ### Example
///
/// ```cpp
/// Element document = vtext("Hello world!");
/// ```
///
/// ### Output
///
/// ```bash
/// H
/// e
/// l
/// l
/// o
///
/// w
/// o
/// r
/// l
/// d
/// !
/// ```
Element vtext(std::string text) {
  return std::make_shared<VText>(std::move(text));
}

/// @brief Display a piece unicode text vertically.
/// @ingroup dom
/// @see ftxui::to_wstring
///
/// ### Example
///
/// ```cpp
/// Element document = vtext(L"Hello world!");
/// ```
///
/// ### Output
///
/// ```bash
/// H
/// e
/// l
/// l
/// o
///
/// w
/// o
/// r
/// l
/// d
/// !
/// ```
Element vtext(std::wstring text) {  // NOLINT
  return std::make_shared<VText>(to_string(text));
}

}  // namespace ftxui
