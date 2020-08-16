#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

using ftxui::Screen;

class Underlined : public NodeDecorator {
 public:
  Underlined(Elements children) : NodeDecorator(std::move(children)) {}
  ~Underlined() override {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).underlined = true;
      }
    }
  }
};

/// @brief Make the underlined element to be underlined.
/// @ingroup dom
Element underlined(Element child) {
  return std::make_shared<Underlined>(unpack(std::move(child)));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
