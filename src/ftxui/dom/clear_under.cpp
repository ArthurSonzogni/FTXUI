#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

using ftxui::Screen;

class ClearUnder : public NodeDecorator {
 public:
  ClearUnder(Elements children) : NodeDecorator(std::move(children)) {}
  ~ClearUnder() override {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y) = Pixel();
      }
    }
    Node::Render(screen);
  }
};

/// @brief Before drawing |child|, clear the pixels below. This is useful in
//         combinaison with dbox.
/// @see ftxui::dbox
/// @ingroup dom
Element clear_under(Element child) {
  return std::make_shared<ClearUnder>(unpack(std::move(child)));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
