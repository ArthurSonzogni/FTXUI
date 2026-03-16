// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for min, max
#include <memory>     // for make_shared
#include <sstream>
#include <string>       // for string, wstring
#include <string_view>  // for string_view
#include <utility>      // for move
#include <vector>       // for vector

#include "ftxui/dom/deprecated.hpp"   // for text, vtext
#include "ftxui/dom/elements.hpp"     // for Element, text, vtext
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/dom/selection.hpp"    // for Selection
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Cell, Screen
#include "ftxui/screen/string.hpp"  // for string_width, Utf8ToGlyphs, to_string

namespace ftxui {

namespace {
using ftxui::Screen;

class Text : public Node {
 public:
  explicit Text(std::string text) : text_(std::move(text)) {}
  explicit Text(std::string_view sv) : Text(std::string(sv)) {}

  void ComputeRequirement() override {
    int max_width = 0;
    int current_width = 0;
    int lines = 1;
    has_selection_ = false;
    selection_rows_.clear();

    for (const auto& cell : Utf8ToGlyphs(text_)) {
      if (cell == "\n") {
        max_width = std::max(max_width, current_width);
        current_width = 0;
        lines++;
      } else {
        current_width++;
      }
    }
    max_width = std::max(max_width, current_width);

    requirement_.min_x = max_width;
    requirement_.min_y = lines;
  }

  void Select(Selection& selection) override {
    if (Box::Intersection(selection.GetBox(), box_).IsEmpty()) {
      return;
    }

    // Split text into lines for per-row selection handling.
    std::vector<std::string> lines;
    {
      std::stringstream ss(text_);
      std::string line;
      while (std::getline(ss, line)) {
        lines.push_back(line);
      }
      if (lines.empty()) {
        lines.push_back("");
      }
    }

    has_selection_ = true;
    selection_rows_.assign(lines.size(), {-1, -1});

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
      const int y = box_.y_min + i;
      if (y > box_.y_max) {
        break;
      }

      const Box row_box{box_.x_min, box_.x_max, y, y};
      if (Box::Intersection(selection.GetBox(), row_box).IsEmpty()) {
        continue;
      }

      const Selection row_sel = selection.SaturateHorizontal(row_box);
      const int sel_start = row_sel.GetBox().x_min;
      const int sel_end = row_sel.GetBox().x_max;
      selection_rows_[i] = {sel_start, sel_end};

      std::stringstream part;
      int x = box_.x_min;
      for (const auto& cell : Utf8ToGlyphs(lines[i])) {
        if (sel_start <= x && x <= sel_end) {
          part << cell;
        }
        x++;
      }
      selection.AddPart(part.str(), y, sel_start, sel_end);
    }
  }

  void Render(Screen& screen) override {
    int x = box_.x_min;
    int y = box_.y_min;
    int line = 0;

    if (y > box_.y_max) {
      return;
    }

    for (const auto& cell : Utf8ToGlyphs(text_)) {
      if (cell == "\n") {
        y++;
        x = box_.x_min;
        line++;
        if (y > box_.y_max) {
          return;
        }
        continue;
      }

      if (x <= box_.x_max) {
        screen.CellAt(x, y).character = cell;

        if (has_selection_ && line < static_cast<int>(selection_rows_.size())) {
          const auto& [sel_start, sel_end] = selection_rows_[line];
          if (sel_start != -1 && x >= sel_start && x <= sel_end) {
            screen.GetSelectionStyle()(screen.CellAt(x, y));
          }
        }
      }
      x++;
    }
  }

 private:
  std::string text_;
  bool has_selection_ = false;
  std::vector<std::pair<int, int>> selection_rows_;
};

class VText : public Node {
 public:
  explicit VText(std::string text)
      : text_(std::move(text)), width_{std::min(string_width(text_), 1)} {}

  explicit VText(std::string_view sv) : VText(std::string(sv)) {}

  void ComputeRequirement() override {
    int max_height = 0;
    int current_height = 0;
    int columns = 1;

    for (const auto& cell : Utf8ToGlyphs(text_)) {
      if (cell == "\n") {
        max_height = std::max(max_height, current_height);
        current_height = 0;
        columns++;
      } else {
        current_height++;
      }
    }
    max_height = std::max(max_height, current_height);

    requirement_.min_x = width_ * columns;
    requirement_.min_y = max_height;
  }

  void Render(Screen& screen) override {
    int x = box_.x_min;
    int y = box_.y_min;
    if (x + width_ - 1 > box_.x_max) {
      return;
    }
    for (const auto& it : Utf8ToGlyphs(text_)) {
      if (it == "\n") {
        x += width_;
        y = box_.y_min;
        if (x + width_ - 1 > box_.x_max) {
          return;
        }
        continue;
      }
      if (y > box_.y_max) {
        continue;
      }
      screen.CellAt(x, y).character = it;
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
Element text(std::string_view text) {
  return std::make_shared<Text>(std::string(text));
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
Element text(std::wstring_view sv) {
  return text(to_string(sv));
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
Element vtext(std::string_view text) {
  return std::make_shared<VText>(std::string(text));
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
Element vtext(std::wstring_view text) {  // NOLINT
  return vtext(to_string(text));
}

}  // namespace ftxui
