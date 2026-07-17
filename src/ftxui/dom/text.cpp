// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for min, max
#include <cstddef>
#include <memory>       // for make_shared
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
  explicit Text(std::string_view text) : glyphs_(Utf8ToGlyphs(text)) {
    int max_width = 0;
    int current_width = 0;
    int lines_count = 1;
    lines_offsets_.push_back(0);

    for (size_t i = 0; i < glyphs_.size(); ++i) {
      if (glyphs_[i] == "\n") {
        max_width = std::max(max_width, current_width);
        current_width = 0;
        lines_count++;
        lines_offsets_.push_back((int)i + 1);
      } else {
        current_width++;
      }
    }
    max_width = std::max(max_width, current_width);
    lines_offsets_.push_back((int)glyphs_.size() + 1);

    requirement_.min_x = max_width;
    requirement_.min_y = lines_count;
  }

  void ComputeRequirement() override {
    // The requirement is computed once in the constructor. Only the selection
    // state must be reset before every frame, because Select() is invoked
    // only while a selection is active.
    has_selection_ = false;
    selection_rows_.clear();
  }

  void Select(Selection& selection) override {
    if (Box::Intersection(selection.GetBox(), box_).IsEmpty()) {
      return;
    }

    has_selection_ = true;
    selection_rows_.assign(lines_offsets_.size() - 1, {-1, -1});

    for (size_t i = 0; i < lines_offsets_.size() - 1; ++i) {
      const int y = box_.y_min + (int)i;
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

      std::string part;
      int x = box_.x_min;
      const int start = lines_offsets_[i];
      const int end = lines_offsets_[i + 1] - 1;
      for (int j = start; j < end; ++j) {
        if (sel_start <= x && x <= sel_end) {
          part += glyphs_[j];
        }
        x++;
      }
      selection.AddPart(std::move(part), y, sel_start, sel_end);
    }
  }

  void Render(Screen& screen) override {
    const auto visible_box = Box::Intersection(screen.stencil, box_);
    if (visible_box.IsEmpty()) {
      return;
    }

    int y = visible_box.y_min;

    const size_t first_line = visible_box.y_min - box_.y_min;
    const size_t last_line = std::min<size_t>(
        visible_box.y_max - box_.y_min + 1, lines_offsets_.size() - 1);

    for (size_t line = first_line; line < last_line; ++line, ++y) {
      int x = box_.x_min;

      for (auto glyph = glyphs_.begin() + lines_offsets_[line];
           glyph != glyphs_.end() && *glyph != "\n"; ++glyph, ++x) {
        if (x > box_.x_max) {
          break;
        }

        auto& cell = screen.CellAt(x, y);
        cell.character = *glyph;

        if (has_selection_ && line < selection_rows_.size()) {
          const auto& [sel_start, sel_end] = selection_rows_[line];
          if (sel_start != -1 && x >= sel_start && x <= sel_end) {
            screen.GetSelectionStyle()(cell);
          }
        }
      }
    }
  }

 private:
  std::vector<std::string> glyphs_;
  std::vector<int> lines_offsets_;
  bool has_selection_ = false;
  std::vector<std::pair<int, int>> selection_rows_;
};

class VText : public Node {
 public:
  explicit VText(std::string_view text) : glyphs_(Utf8ToGlyphs(text)) {
    for (const auto& g : glyphs_) {
      if (g != "\n") {
        width_ = 1;
        break;
      }
    }
  }

  void ComputeRequirement() override {
    int max_height = 0;
    int current_height = 0;
    int columns = 1;

    for (const auto& cell : glyphs_) {
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
    for (const auto& it : glyphs_) {
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
  std::vector<std::string> glyphs_;
  int width_ = 0;
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
Element text(std::wstring_view text) {
  return ftxui::text(to_string(text));
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
  return std::make_shared<VText>(text);
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
