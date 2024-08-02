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

Region selectedRegion;

namespace {
class Selectable : public NodeDecorator {
 public:
  using NodeDecorator::NodeDecorator;
  Selectable(Element child, std::function<void(const std::string)> onSelectionChange)
      : NodeDecorator(std::move(child)), onSelectionChange_(onSelectionChange) {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    std::string selectedText = "";
    for (int y = std::min(selectedRegion.starty, selectedRegion.endy); y <= std::max(selectedRegion.starty, selectedRegion.endy); ++y) {
      for (int x = std::min(selectedRegion.startx, selectedRegion.endx); x <= std::max(selectedRegion.startx, selectedRegion.endx)-1; ++x) {
        screen.PixelAt(x, y).inverted ^= true;
        selectedText += screen.PixelAt(x, y).character;
      }
    }

    if(selectedRegion.changed == true) {
      selectedRegion.changed = false;;
      onSelectionChange_(selectedText);
    }
  }

private:
  std::function<void(const std::string)> onSelectionChange_;
};
}  // namespace

/// @brief Add a filter that will invert the foreground and the background
/// colors.
/// @ingroup dom

Element selectable(std::function<void(const std::string)> onSelectionChange, Element child) {
  return std::make_shared<Selectable>(std::move(child), onSelectionChange);
}

Decorator selectable(std::function<void(const std::string)> onSelectionChange) {
  return [onSelectionChange](Element child) { return selectable(onSelectionChange, std::move(child)); };
}

bool selectableCatchEvent(Event event) {

    if (event.is_mouse()) {
      auto& mouse = event.mouse();
      if (mouse.button == Mouse::Left) {

        if (mouse.motion == Mouse::Pressed) {
          selectedRegion.startx = mouse.x;
          selectedRegion.starty = mouse.y;
          selectedRegion.endx = mouse.x;
          selectedRegion.endy = mouse.y;
          selectedRegion.changed = true;
        } else if (mouse.motion == Mouse::Released) {
          selectedRegion.endx = mouse.x;
          selectedRegion.endy = mouse.y;
          selectedRegion.changed = true;
        } else if (mouse.motion == Mouse::Moved) {
          selectedRegion.endx = mouse.x;
          selectedRegion.endy = mouse.y;
          selectedRegion.changed = true;
        }

        return true;
      }
    }

    return false;
}

}  // namespace ftxui
