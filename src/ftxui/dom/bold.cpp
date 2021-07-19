#include <memory>   // for make_shared
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"        // for Element, bold
#include "ftxui/dom/node.hpp"            // for Node
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

class Bold : public NodeDecorator {
 public:
  using NodeDecorator::NodeDecorator;

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).bold = true;
      }
    }
    Node::Render(screen);
  }
};

/// @brief Use a bold font, for elements with more emphasis.
/// @ingroup dom
Element bold(Element child) {
  return std::make_shared<Bold>(std::move(child));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
