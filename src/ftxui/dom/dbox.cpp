// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for max
#include <cstddef>    // for size_t
#include <memory>     // for __shared_ptr_access, shared_ptr, make_shared
#include <utility>    // for move
#include <vector>

#include "ftxui/dom/elements.hpp"     // for Element, Elements, dbox
#include "ftxui/dom/node.hpp"         // for Node, Elements
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/pixel.hpp"     // for Pixel

namespace ftxui {

namespace {
class DBox : public Node {
 public:
  explicit DBox(Elements children) : Node(std::move(children)) {}

  void ComputeRequirement() override {
    requirement_ = Requirement{};
    for (auto& child : children_) {
      child->ComputeRequirement();

      // Propagate the focused requirement.
      if (requirement_.focused.Prefer(child->requirement().focused)) {
        requirement_.focused = child->requirement().focused;
      }

      // Extend the min_x and min_y to contain all the children
      requirement_.min_x =
          std::max(requirement_.min_x, child->requirement().min_x);
      requirement_.min_y =
          std::max(requirement_.min_y, child->requirement().min_y);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    for (auto& child : children_) {
      child->SetBox(box);
    }
  }

  void Render(Screen& screen) override {
    if (children_.size() <= 1) {
      Node::Render(screen);
      return;
    }

    const int width = box_.x_max - box_.x_min + 1;
    const int height = box_.y_max - box_.y_min + 1;
    std::vector<Pixel> pixels(std::size_t(width * height));

    for (auto& child : children_) {
      child->Render(screen);

      // Accumulate the pixels
      Pixel* acc = pixels.data();
      for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
          auto& pixel = screen.PixelAt(x + box_.x_min, y + box_.y_min);
          acc->background_color =
              Color::Blend(acc->background_color, pixel.background_color);
          acc->automerge = pixel.automerge || acc->automerge;
          if (pixel.character.empty()) {
            acc->foreground_color =
                Color::Blend(acc->foreground_color, pixel.background_color);
          } else {
            acc->blink = pixel.blink;
            acc->bold = pixel.bold;
            acc->dim = pixel.dim;
            acc->inverted = pixel.inverted;
            acc->italic = pixel.italic;
            acc->underlined = pixel.underlined;
            acc->underlined_double = pixel.underlined_double;
            acc->strikethrough = pixel.strikethrough;
            acc->hyperlink = pixel.hyperlink;
            acc->character = pixel.character;
            acc->foreground_color = pixel.foreground_color;
          }
          ++acc;  // NOLINT

          pixel = Pixel();
        }
      }
    }

    // Render the accumulated pixels:
    Pixel* acc = pixels.data();
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        screen.PixelAt(x + box_.x_min, y + box_.y_min) = *acc++;  // NOLINT
      }
    }
  }
};
}  // namespace

/// @brief Stack several element on top of each other.
/// @param children_ The input element.
/// @return The right aligned element.
/// @ingroup dom
Element dbox(Elements children_) {
  return std::make_shared<DBox>(std::move(children_));
}

}  // namespace ftxui
