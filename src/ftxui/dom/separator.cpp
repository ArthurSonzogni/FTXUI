#include <memory>  // for make_shared
#include <string>  // for string

#include "ftxui/dom/elements.hpp"     // for Element, separator
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Pixel, Screen

namespace ftxui {

using ftxui::Screen;

class Separator : public Node {
 public:
  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    bool is_column = (box_.x_max == box_.x_min);
    bool is_line = (box_.y_min == box_.y_max);

    std::string c = "+";
    if (is_line && !is_column)
      c = "─";
    else
      c = "│";

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).character = c;
      }
    }
  }
};

class SeparatorWithPixel : public Separator {
 public:
  SeparatorWithPixel(Pixel pixel) : pixel_(pixel) {}
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

Element separator() {
  return std::make_shared<Separator>();
}

Element separator(Pixel pixel) {
  return std::make_shared<SeparatorWithPixel>(pixel);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
