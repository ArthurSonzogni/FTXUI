// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>   // for make_shared
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"        // for Element, inverted
#include "ftxui/dom/node.hpp"            // for Node
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

namespace {
class Selected : public NodeDecorator {
 public:
  using NodeDecorator::NodeDecorator;
  Selected(Element child, int &start, int &end)
      : NodeDecorator(std::move(child)), startx_(start), endx_(end) {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = startx_; x <= endx_; ++x) {
        screen.PixelAt(x, y).inverted ^= true;
      }
    }
  }

private:
  int &startx_;
  int &endx_;
};
}  // namespace

/// @brief Add a filter that will invert the foreground and the background
/// colors.
/// @ingroup dom

Element selected(int &start, int &end, Element child) {
  return std::make_shared<Selected>(std::move(child), start, end);
}

Decorator selected(int &start, int &end) {
  return [&start, &end](Element child) { return selected(start, end, std::move(child)); };
}

}  // namespace ftxui
