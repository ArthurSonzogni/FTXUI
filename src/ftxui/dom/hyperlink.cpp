#include <memory>   // for make_shared
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"  // for Element, Decorator, bgcolor, hyperlink
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

class Hyperlink : public NodeDecorator {
 public:
  Hyperlink(Element child, std::string link)
      : NodeDecorator(std::move(child)), link_(link) {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).hyperlink = link_;
      }
    }
    NodeDecorator::Render(screen);
  }

  std::string link_;
};

/// @brief Make the rendered area clickable using a web browser.
///        The link will be opened when the user click on the element.
///        This is supported only on a limited set of terminal emulator.
///        List: https://github.com/Alhadis/OSC8-Adoption/
/// @param link The link
/// @param child The input element.
/// @return The output element with the link.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document =
///   hyperlink("https://github.com/ArthurSonzogni/FTXUI", "link");
/// ```
Element hyperlink(std::string link, Element child) {
  return std::make_shared<Hyperlink>(std::move(child), link);
}

/// @brief Decorate using an hyperlink.
///        The link will be opened when the user click on the element.
///        This is supported only on a limited set of terminal emulator.
///        List: https://github.com/Alhadis/OSC8-Adoption/
/// @param link The link to redirect the users to.
/// @return The Decorator applying the hyperlink.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document =
///   text("red") | hyperlink("https://github.com/Arthursonzogni/FTXUI");
/// ```
Decorator hyperlink(std::string link) {
  return [link](Element child) { return hyperlink(link, std::move(child)); };
}

}  // namespace ftxui

// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
