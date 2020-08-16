#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

class BgColor : public NodeDecorator {
 public:
  BgColor(Elements children, Color color)
      : NodeDecorator(std::move(children)), color_(color) {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).background_color = color_;
      }
    }
    NodeDecorator::Render(screen);
  }

  Color color_;
};

class FgColor : public NodeDecorator {
 public:
  FgColor(Elements children, Color color)
      : NodeDecorator(std::move(children)), color_(color) {}
  ~FgColor() override {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).foreground_color = color_;
      }
    }
    NodeDecorator::Render(screen);
  }

  Color color_;
};

/// @brief Set the foreground color of an element.
/// @param color The color of the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = color(Color::Green, text(L"Success")),
/// ```
Element color(Color color, Element child) {
  return std::make_shared<FgColor>(unpack(std::move(child)), color);
}

/// @brief Set the background color of an element.
/// @param color The color of the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = bgcolor(Color::Green, text(L"Success")),
/// ```
Element bgcolor(Color color, Element child) {
  return std::make_shared<BgColor>(unpack(std::move(child)), color);
}

/// @brief Decorate using a foreground color.
/// @param c The foreground color to be applied.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text(L"red") | color(Color::Red);
/// ```
Decorator color(Color c) {
  return [c](Element child) { return color(c, std::move(child)); };
}

/// @brief Decorate using a background color.
/// @param color The background color to be applied.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text(L"red") | bgcolor(Color::Red);
/// ```
Decorator bgcolor(Color color) {
  return [color](Element child) { return bgcolor(color, std::move(child)); };
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
