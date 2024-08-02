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
  Selected(Element child, Region &selection, std::string &destination)
      : NodeDecorator(std::move(child)), selection_(selection), destination_(destination) {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    destination_ = "";
    for (int y = std::min(selection_.starty, selection_.endy); y <= std::max(selection_.starty, selection_.endy); ++y) {
      for (int x = std::min(selection_.startx, selection_.endx); x <= std::max(selection_.startx, selection_.endx)-1; ++x) {
        screen.PixelAt(x, y).inverted ^= true;
        destination_ += screen.PixelAt(x, y).character;
      }
    }
  }

private:
  Region &selection_;
  std::string &destination_;
};
}  // namespace

/// @brief Add a filter that will invert the foreground and the background
/// colors.
/// @ingroup dom

Element selected(Region &selection, std::string &destination, Element child) {
  return std::make_shared<Selected>(std::move(child), selection, destination);
}

Decorator selected(Region &selection, std::string &destination) {
  return [&selection, &destination](Element child) { return selected(selection, destination, std::move(child)); };
}

}  // namespace ftxui
