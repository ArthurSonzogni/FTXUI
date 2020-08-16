#include <algorithm>
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/string.hpp"

namespace ftxui {

using ftxui::Screen;

class Text : public Node {
 public:
  Text(std::wstring text) : Node(), text_(text) {}
  ~Text() {}

  void ComputeRequirement() override {
    requirement_.min_x = wstring_width(text_);
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    int x = box_.x_min;
    int y = box_.y_min;
    if (y > box_.y_max)
      return;
    for (wchar_t c : text_) {
      if (x > box_.x_max)
        return;
      screen.at(x, y) = c;
      x += wchar_width(c);
    }
  }

 private:
  std::wstring text_;
};

class VText : public Node {
 public:
  VText(std::wstring text) : Node(), text_(text) {
    for (auto& c : text_)
      width_ = std::max(width_, wchar_width(c));
  }
  ~VText() {}

  void ComputeRequirement() override {
    requirement_.min_x = width_;
    requirement_.min_y = text_.size();
  }

  void Render(Screen& screen) override {
    int x = box_.x_min;
    int y = box_.y_min;
    if (x + width_ - 1 > box_.x_max)
      return;
    for (wchar_t c : text_) {
      if (y > box_.y_max)
        return;
      screen.at(x, y) = c;
      y += 1;
    }
  }

 private:
  std::wstring text_;
  int width_ = 1;
};

/// @brief Display a pieve of unicode text.
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
Element text(std::wstring text) {
  return std::make_shared<Text>(text);
}

/// @brief Display a pieve of unicode text vertically.
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
Element vtext(std::wstring text) {
  return std::make_shared<VText>(text);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
