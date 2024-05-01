// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for max
#include <memory>     // for make_shared, __shared_ptr_access
#include <string>     // for string
#include <utility>    // for move

#include "ftxui/dom/elements.hpp"  // for Element, vscroll_indicator, hscroll_indicator
#include "ftxui/dom/node.hpp"            // for Node, Elements
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/dom/requirement.hpp"     // for Requirement
#include "ftxui/screen/box.hpp"          // for Box
#include "ftxui/screen/screen.hpp"       // for Screen, Pixel

namespace ftxui {

/// @brief Display a vertical scrollbar to the right.
/// colors.
/// @ingroup dom
Element vscroll_indicator(Element child) {
  class Impl : public NodeDecorator {
    using NodeDecorator::NodeDecorator;

    void ComputeRequirement() override {
      NodeDecorator::ComputeRequirement();
      requirement_ = children_[0]->requirement();
      requirement_.min_x++;
    }

    void SetBox(Box box) override {
      box_ = box;
      box.x_max--;
      children_[0]->SetBox(box);
    }

    void Render(Screen& screen) final {
      NodeDecorator::Render(screen);

      const Box& stencil = screen.stencil;

      const int size_inner = box_.y_max - box_.y_min;
      if (size_inner <= 0) {
        return;
      }
      const int size_outter = stencil.y_max - stencil.y_min + 1;
      if (size_outter >= size_inner) {
        return;
      }

      int size = 2 * size_outter * size_outter / size_inner;
      size = std::max(size, 1);

      const int start_y =
          2 * stencil.y_min +  //
          2 * (stencil.y_min - box_.y_min) * size_outter / size_inner;

      const int x = stencil.x_max;
      for (int y = stencil.y_min; y <= stencil.y_max; ++y) {
        const int y_up = 2 * y + 0;
        const int y_down = 2 * y + 1;
        const bool up = (start_y <= y_up) && (y_up <= start_y + size);
        const bool down = (start_y <= y_down) && (y_down <= start_y + size);

        const char* c = up ? (down ? "┃" : "╹") : (down ? "╻" : " ");  // NOLINT
        screen.PixelAt(x, y).character = c;
      }
    }
  };
  return std::make_shared<Impl>(std::move(child));
}

/// @brief Display an horizontal scrollbar to the bottom.
/// colors.
/// @ingroup dom
Element hscroll_indicator(Element child) {
  class Impl : public NodeDecorator {
    using NodeDecorator::NodeDecorator;

    void ComputeRequirement() override {
      NodeDecorator::ComputeRequirement();
      requirement_ = children_[0]->requirement();
      requirement_.min_y++;
    }

    void SetBox(Box box) override {
      box_ = box;
      box.y_max--;
      children_[0]->SetBox(box);
    }

    void Render(Screen& screen) final {
      NodeDecorator::Render(screen);

      const Box& stencil = screen.stencil;

      const int size_inner = box_.x_max - box_.x_min;
      if (size_inner <= 0) {
        return;
      }
      const int size_outter = stencil.x_max - stencil.x_min + 1;
      if (size_outter >= size_inner) {
        return;
      }

      int size = 2 * size_outter * size_outter / size_inner;
      size = std::max(size, 1);

      const int start_x =
          2 * stencil.x_min +  //
          2 * (stencil.x_min - box_.x_min) * size_outter / size_inner;

      const int y = stencil.y_max;
      for (int x = stencil.x_min; x <= stencil.x_max; ++x) {
        const int x_left = 2 * x + 0;
        const int x_right = 2 * x + 1;
        const bool left = (start_x <= x_left) && (x_left <= start_x + size);
        const bool right = (start_x <= x_right) && (x_right <= start_x + size);

        const char* c =
            left ? (right ? "─" : "╴") : (right ? "╶" : " ");  // NOLINT
        screen.PixelAt(x, y).character = c;
      }
    }
  };
  return std::make_shared<Impl>(std::move(child));
}

}  // namespace ftxui
