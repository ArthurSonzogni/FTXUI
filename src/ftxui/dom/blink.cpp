#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

class Blink : public NodeDecorator {
 public:
  Blink(Elements children) : NodeDecorator(std::move(children)) {}
  ~Blink() override {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).blink = true;
      }
    }
  }
};

/// @brief The text drawn alternates in between visible and hidden.
/// @ingroup dom
Element blink(Element child) {
  return std::make_shared<Blink>(unpack(std::move(child)));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
