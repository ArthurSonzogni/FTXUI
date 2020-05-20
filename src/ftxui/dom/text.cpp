// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/dom/node.hpp"
#include "ftxui/screen/string.hpp"

namespace ftxui {

using ftxui::Screen;

class Text : public Node {
 public:
  Text(std::wstring text) : Node(), text_(text) {}
  ~Text() {}

  void ComputeRequirement() override {
    requirement_.min.x = wstring_width(text_);
    requirement_.min.y = 1;
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

Element text(std::wstring text) {
  return std::make_shared<Text>(text);
}

}  // namespace ftxui
