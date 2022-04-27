#include <algorithm>  // for max
#include <array>      // for array
#include <memory>     // for allocator, make_shared, __shared_ptr_access
#include <string>     // for basic_string, string
#include <utility>    // for move
#include <vector>     // for __alloc_traits<>::value_type

#include "ftxui/dom/elements.hpp"  // for unpack, Element, Decorator, BorderStyle, ROUNDED, Elements, DOUBLE, EMPTY, HEAVY, LIGHT, border, borderDouble, borderEmpty, borderHeavy, borderLight, borderRounded, borderStyled, borderWith, window
#include "ftxui/dom/node.hpp"      // for Node, Elements
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Pixel, Screen

namespace ftxui {

using Charset = std::array<std::string, 6>;  // NOLINT
using Charsets = std::array<Charset, 6>;     // NOLINT
// NOLINTNEXTLINE
static Charsets simple_border_charset = {
    Charset{"┌", "┐", "└", "┘", "─", "│"},
    Charset{"┏", "┓", "┗", "┛", "━", "┃"},
    Charset{"╔", "╗", "╚", "╝", "═", "║"},
    Charset{"╭", "╮", "╰", "╯", "─", "│"},
    Charset{" ", " ", " ", " ", " ", " "},
};

// For reference, here is the charset for normal border:
class Border : public Node {
 public:
  Border(Elements children, BorderStyle style)
      : Node(std::move(children)),
        charset_(simple_border_charset[style]) {}  // NOLINT

  const Charset& charset_;  // NOLINT

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children_[0]->requirement();
    requirement_.min_x += 2;
    requirement_.min_y += 2;
    if (children_.size() == 2) {
      requirement_.min_x =
          std::max(requirement_.min_x, children_[1]->requirement().min_x + 2);
    }
    requirement_.selected_box.x_min++;
    requirement_.selected_box.x_max++;
    requirement_.selected_box.y_min++;
    requirement_.selected_box.y_max++;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    if (children_.size() == 2) {
      Box title_box;
      title_box.x_min = box.x_min + 1;
      title_box.x_max = box.x_max - 1;
      title_box.y_min = box.y_min;
      title_box.y_max = box.y_min;
      children_[1]->SetBox(title_box);
    }
    box.x_min++;
    box.x_max--;
    box.y_min++;
    box.y_max--;
    children_[0]->SetBox(box);
  }

  void Render(Screen& screen) override {
    // Draw content.
    children_[0]->Render(screen);

    // Draw the border.
    if (box_.x_min >= box_.x_max || box_.y_min >= box_.y_max) {
      return;
    }

    screen.at(box_.x_min, box_.y_min) = charset_[0];  // NOLINT
    screen.at(box_.x_max, box_.y_min) = charset_[1];  // NOLINT
    screen.at(box_.x_min, box_.y_max) = charset_[2];  // NOLINT
    screen.at(box_.x_max, box_.y_max) = charset_[3];  // NOLINT

    for (int x = box_.x_min + 1; x < box_.x_max; ++x) {
      Pixel& p1 = screen.PixelAt(x, box_.y_min);
      Pixel& p2 = screen.PixelAt(x, box_.y_max);
      p1.character = charset_[4];  // NOLINT
      p2.character = charset_[4];  // NOLINT
      p1.automerge = true;
      p2.automerge = true;
    }
    for (int y = box_.y_min + 1; y < box_.y_max; ++y) {
      Pixel& p3 = screen.PixelAt(box_.x_min, y);
      Pixel& p4 = screen.PixelAt(box_.x_max, y);
      p3.character = charset_[5];  // NOLINT
      p4.character = charset_[5];  // NOLINT
      p3.automerge = true;
      p4.automerge = true;
    }

    // Draw title.
    if (children_.size() == 2) {
      children_[1]->Render(screen);
    }
  }
};

// For reference, here is the charset for normal border:
class BorderPixel : public Node {
 public:
  BorderPixel(Elements children, Pixel pixel)
      : Node(std::move(children)), pixel_(std::move(pixel)) {}

 private:
  Pixel pixel_;

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children_[0]->requirement();
    requirement_.min_x += 2;
    requirement_.min_y += 2;
    if (children_.size() == 2) {
      requirement_.min_x =
          std::max(requirement_.min_x, children_[1]->requirement().min_x + 2);
    }
    requirement_.selected_box.x_min++;
    requirement_.selected_box.x_max++;
    requirement_.selected_box.y_min++;
    requirement_.selected_box.y_max++;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    if (children_.size() == 2) {
      Box title_box;
      title_box.x_min = box.x_min + 1;
      title_box.x_max = box.x_max - 1;
      title_box.y_min = box.y_min;
      title_box.y_max = box.y_min;
      children_[1]->SetBox(title_box);
    }
    box.x_min++;
    box.x_max--;
    box.y_min++;
    box.y_max--;
    children_[0]->SetBox(box);
  }

  void Render(Screen& screen) override {
    // Draw content.
    children_[0]->Render(screen);

    // Draw the border.
    if (box_.x_min >= box_.x_max || box_.y_min >= box_.y_max) {
      return;
    }

    screen.PixelAt(box_.x_min, box_.y_min) = pixel_;
    screen.PixelAt(box_.x_max, box_.y_min) = pixel_;
    screen.PixelAt(box_.x_min, box_.y_max) = pixel_;
    screen.PixelAt(box_.x_max, box_.y_max) = pixel_;

    for (int x = box_.x_min + 1; x < box_.x_max; ++x) {
      screen.PixelAt(x, box_.y_min) = pixel_;
      screen.PixelAt(x, box_.y_max) = pixel_;
    }
    for (int y = box_.y_min + 1; y < box_.y_max; ++y) {
      screen.PixelAt(box_.x_min, y) = pixel_;
      screen.PixelAt(box_.x_max, y) = pixel_;
    }
  }
};

/// @brief Draw a border around the element.
/// @ingroup dom
/// @see border
/// @see borderLight
/// @see borderDouble
/// @see borderHeavy
/// @see borderEmpty
/// @see borderRounded
///
/// Add a border around an element
///
/// ### Example
///
/// ```cpp
/// // Use 'border' as a function...
/// Element document = border(text("The element"));
///
/// // ...Or as a 'pipe'.
/// Element document = text("The element") | border;
/// ```
///
/// ### Output
///
/// ```bash
/// ┌───────────┐
/// │The element│
/// └───────────┘
/// ```
Element border(Element child) {
  return std::make_shared<Border>(unpack(std::move(child)), ROUNDED);
}

/// @brief Same as border but with a constant Pixel around the element.
/// @ingroup dom
/// @see border
Decorator borderWith(const Pixel& pixel) {
  return [pixel](Element child) {
    return std::make_shared<BorderPixel>(unpack(std::move(child)), pixel);
  };
}

/// @brief Same as border but with different styles.
/// @ingroup dom
/// @see border
Decorator borderStyled(BorderStyle style) {
  return [style](Element child) {
    return std::make_shared<Border>(unpack(std::move(child)), style);
  };
}

/// @brief Draw a light border around the element.
/// @ingroup dom
/// @see border
/// @see borderLight
/// @see borderDouble
/// @see borderHeavy
/// @see borderRounded
/// @see borderEmpty
/// @see borderStyled
/// @see borderWith
///
/// Add a border around an element
///
/// ### Example
///
/// ```cpp
/// // Use 'borderLight' as a function...
/// Element document = borderLight(text("The element"));
///
/// // ...Or as a 'pipe'.
/// Element document = text("The element") | borderLight;
/// ```
///
/// ### Output
///
/// ```bash
/// ┌──────────────┐
/// │The element   │
/// └──────────────┘
/// ```
Element borderLight(Element child) {
  return std::make_shared<Border>(unpack(std::move(child)), LIGHT);
}

/// @brief Draw a heavy border around the element.
/// @ingroup dom
/// @see border
/// @see borderLight
/// @see borderDouble
/// @see borderHeavy
/// @see borderRounded
/// @see borderEmpty
/// @see borderStyled
/// @see borderWith
///
/// Add a border around an element
///
/// ### Example
///
/// ```cpp
/// // Use 'borderHeavy' as a function...
/// Element document = borderHeavy(text("The element"));
///
/// // ...Or as a 'pipe'.
/// Element document = text("The element") | borderHeavy;
/// ```
///
/// ### Output
///
/// ```bash
/// ┏━━━━━━━━━━━━━━┓
/// ┃The element   ┃
/// ┗━━━━━━━━━━━━━━┛
/// ```
Element borderHeavy(Element child) {
  return std::make_shared<Border>(unpack(std::move(child)), HEAVY);
}

/// @brief Draw a double border around the element.
/// @ingroup dom
/// @see border
/// @see borderLight
/// @see borderDouble
/// @see borderHeavy
/// @see borderRounded
/// @see borderEmpty
/// @see borderStyled
/// @see borderWith
///
/// Add a border around an element
///
/// ### Example
///
/// ```cpp
/// // Use 'borderDouble' as a function...
/// Element document = borderDouble(text("The element"));
///
/// // ...Or as a 'pipe'.
/// Element document = text("The element") | borderDouble;
/// ```
///
/// ### Output
///
/// ```bash
/// ╔══════════════╗
/// ║The element   ║
/// ╚══════════════╝
/// ```
Element borderDouble(Element child) {
  return std::make_shared<Border>(unpack(std::move(child)), DOUBLE);
}

/// @brief Draw a rounded border around the element.
/// @ingroup dom
/// @see border
/// @see borderLight
/// @see borderDouble
/// @see borderHeavy
/// @see borderRounded
/// @see borderEmpty
/// @see borderStyled
/// @see borderWith
///
/// Add a border around an element
///
/// ### Example
///
/// ```cpp
/// // Use 'borderRounded' as a function...
/// Element document = borderRounded(text("The element"));
///
/// // ...Or as a 'pipe'.
/// Element document = text("The element") | borderRounded;
/// ```
///
/// ### Output
///
/// ```bash
/// ╭──────────────╮
/// │The element   │
/// ╰──────────────╯
/// ```
Element borderRounded(Element child) {
  return std::make_shared<Border>(unpack(std::move(child)), ROUNDED);
}

/// @brief Draw an empty border around the element.
/// @ingroup dom
/// @see border
/// @see borderLight
/// @see borderDouble
/// @see borderHeavy
/// @see borderRounded
/// @see borderEmpty
/// @see borderStyled
/// @see borderWith
///
/// Add a border around an element
///
/// ### Example
///
/// ```cpp
/// // Use 'borderRounded' as a function...
/// Element document = borderRounded(text("The element"));
///
/// // ...Or as a 'pipe'.
/// Element document = text("The element") | borderRounded;
/// ```
///
/// ### Output
///
/// ```bash
///
///  The element
///
/// ```
Element borderEmpty(Element child) {
  return std::make_shared<Border>(unpack(std::move(child)), EMPTY);
}

/// @brief Draw window with a title and a border around the element.
/// @param title The title of the window.
/// @param content The element to be wrapped.
/// @ingroup dom
/// @see border
///
/// ### Example
///
/// ```cpp
/// Element document = window(text("Title"),
///                           text("content")
///                    );
/// ```
///
/// ### Output
///
/// ```bash
/// ┌Title──┐
/// │content│
/// └───────┘
/// ```
Element window(Element title, Element content) {
  return std::make_shared<Border>(unpack(std::move(content), std::move(title)),
                                  ROUNDED);
}
}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
