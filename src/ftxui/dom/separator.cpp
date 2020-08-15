#include "ftxui/dom/node.hpp"

namespace ftxui {

using ftxui::Screen;

class Separator : public Node {
 public:
  Separator() {}
  ~Separator() override {}
  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    bool is_column = (box_.x_max == box_.x_min);
    bool is_line = (box_.y_min == box_.y_max);

    wchar_t c = U'+';
    if (is_line && !is_column)
      c = U'─';
    else
      c = U'│';

    Pixel p;
    p.character = c;
    RenderWithPixel(screen, p);
  }

  void RenderWithPixel(Screen& screen, Pixel pixel) {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y) = pixel;
      }
    }
  }
};

class SeparatorWithPixel : public Separator {
 public:
  SeparatorWithPixel(Pixel p) : p(p) {}
  ~SeparatorWithPixel() override {}
  void Render(Screen& screen) override { RenderWithPixel(screen, p); }
  Pixel p;
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
