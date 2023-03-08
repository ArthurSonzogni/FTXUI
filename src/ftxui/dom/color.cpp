#include <memory>   // for make_shared
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"  // for Element, Decorator, bgcolor, color
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/color.hpp"        // for Color
#include "ftxui/screen/screen.hpp"       // for Pixel, Screen

namespace ftxui {

class BgColor : public NodeDecorator {
 public:
  BgColor(Element child, Color color)
      : NodeDecorator(std::move(child)), color_(color) {}

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
  FgColor(Element child, Color color)
      : NodeDecorator(std::move(child)), color_(color) {}

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

class GradBgColor : public NodeDecorator {
 public:
  GradBgColor(Element child, Color color_begin, Color color_end)
      : NodeDecorator(std::move(child)),
        color_begin_(color_begin),
        color_end_(color_end) {}

  void Render(Screen& screen) override {
    float max_steps = (box_.y_max - box_.y_min) + (box_.x_max - box_.x_min);
    float step = 0;

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        float target = step / max_steps;

        screen.PixelAt(x, y).background_color =
            Color::Interpolate(target, color_begin_, color_end_);

        step++;
      }
    }
    NodeDecorator::Render(screen);
  }

  Color color_begin_, color_end_;
};

class GradFgColor : public NodeDecorator {
 public:
  GradFgColor(Element child, Color color_begin, Color color_end)
      : NodeDecorator(std::move(child)),
        color_begin_(color_begin),
        color_end_(color_end) {}

  void Render(Screen& screen) override {
    float max_steps = (box_.y_max - box_.y_min) + (box_.x_max - box_.x_min);
    float step = 0;

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        float target = step / max_steps;

        screen.PixelAt(x, y).foreground_color =
            Color::Interpolate(target, color_begin_, color_end_);

        step++;
      }
    }
    NodeDecorator::Render(screen);
  }

  Color color_begin_, color_end_;
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
/// Element document = color(Color::Green, text("Success")),
/// ```
Element color(Color color, Element child) {
  return std::make_shared<FgColor>(std::move(child), color);
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
/// Element document = bgcolor(Color::Green, text("Success")),
/// ```
Element bgcolor(Color color, Element child) {
  return std::make_shared<BgColor>(std::move(child), color);
}

/// @brief Set the foreground color of an element with gradient effect.
/// @param color_begin The first color of the output element.
/// @param color_end The last color of the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = colorgrad(Color::GreenLight, Color::Green, text("Success")),
/// ```
Element colorgrad(Color color_begin, Color color_end, Element child) {
  return std::make_shared<GradFgColor>(std::move(child), color_begin,
                                       color_end);
}

/// @brief Set the background color of an element with gradient effect.
/// @param color_begin The first color of the output element.
/// @param color_end The last color of the output element.
/// @param child The input element.
/// @return The output element colored.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = bgcolorgrad(Color::GreenLight, Color::Green, text("Success")),
/// ```
Element bgcolorgrad(Color color_begin, Color color_end, Element child) {
  return std::make_shared<GradBgColor>(std::move(child), color_begin,
                                       color_end);
}

/// @brief Decorate using a foreground color.
/// @param c The foreground color to be applied.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | color(Color::Red);
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
/// Element document = text("red") | bgcolor(Color::Red);
/// ```
Decorator bgcolor(Color color) {
  return [color](Element child) { return bgcolor(color, std::move(child)); };
}

/// @brief Decorate using a foreground color with gradient effect.
/// @param color The foreground color to be applied first.
/// @param color The foreground color to be applied last.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | colorgrad(Color::RedLight, Color::Red);
/// ```
Decorator colorgrad(Color color_begin, Color color_end) {
  return [color_begin, color_end](Element child) {
    return colorgrad(color_begin, color_end, std::move(child));
  };
}

/// @brief Decorate using a background color with gradient effect.
/// @param color The background color to be applied first.
/// @param color The background color to be applied last.
/// @return The Decorator applying the color.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// Element document = text("red") | bgcolorgrad(Color::RedLight, Color::Red);
/// ```
Decorator bgcolorgrad(Color color_begin, Color color_end) {
  return [color_begin, color_end](Element child) {
    return bgcolorgrad(color_begin, color_end, std::move(child));
  };
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
