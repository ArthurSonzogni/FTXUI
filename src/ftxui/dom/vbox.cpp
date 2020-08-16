#include <algorithm>
#include <iostream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class VBox : public Node {
 public:
  VBox(Elements children) : Node(std::move(children)) {}
  ~VBox() {}

  void ComputeRequirement() override {
    requirement_.min_x = 0;
    requirement_.min_y = 0;
    requirement_.flex_grow_x = 0;
    requirement_.flex_grow_y = 0;
    requirement_.flex_shrink_x = 0;
    requirement_.flex_shrink_y = 0;
    for (auto& child : children) {
      child->ComputeRequirement();
      if (requirement_.selection < child->requirement().selection) {
        requirement_.selection = child->requirement().selection;
        requirement_.selected_box = child->requirement().selected_box;
        requirement_.selected_box.y_min += requirement_.min_y;
        requirement_.selected_box.y_max += requirement_.min_y;
      }
      requirement_.min_y += child->requirement().min_y;
      requirement_.min_x =
          std::max(requirement_.min_x, child->requirement().min_x);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    int space = box.y_max - box.y_min + 1;
    int extra_space = space - requirement_.min_y;

    int size = 0;
    int flex_grow_sum = 0;
    int flex_shrink_sum = 0;
    int flex_shrink_size = 0;
    for (auto& child : children) {
      const Requirement& r = child->requirement();
      flex_grow_sum += r.flex_grow_y;
      flex_shrink_sum += r.min_y * r.flex_shrink_y;
      if (r.flex_shrink_y) {
        flex_shrink_size += r.min_y;
      }
      size += r.min_y;
    }

    if (extra_space >= 0)
      SetBoxGrow(box, extra_space, flex_grow_sum);
    else if (flex_shrink_size + extra_space >= 0)
      SetBoxShrinkEasy(box, extra_space, flex_shrink_sum);
    else
      SetBoxShrinkHard(box, extra_space + flex_shrink_size,
                       size - flex_shrink_size);
  }

  void SetBoxGrow(Box box, int extra_space, int flex_grow_sum) {
    int y = box.y_min;
    for (auto& child : children) {
      Box child_box = box;
      const Requirement& r = child->requirement();

      int added_space =
          extra_space * r.flex_grow_y / std::max(flex_grow_sum, 1);
      extra_space -= added_space;
      flex_grow_sum -= r.flex_grow_y;

      child_box.y_min = y;
      child_box.y_max = y + r.min_y + added_space - 1;

      child->SetBox(child_box);
      y = child_box.y_max + 1;
    }
  }

  void SetBoxShrinkEasy(Box box, int extra_space, int flex_shrink_sum) {
    int y = box.y_min;
    for (auto& child : children) {
      Box child_box = box;
      const Requirement& r = child->requirement();

      int added_space = extra_space * r.min_y * r.flex_shrink_y /
                        std::max(flex_shrink_sum, 1);
      extra_space -= added_space;
      flex_shrink_sum -= r.flex_shrink_y * r.min_y;

      child_box.y_min = y;
      child_box.y_max = y + r.min_y + added_space - 1;

      child->SetBox(child_box);
      y = child_box.y_max + 1;
    }
  }

  void SetBoxShrinkHard(Box box, int extra_space, int size) {
    int y = box.y_min;
    for (auto& child : children) {
      Box child_box = box;
      const Requirement& r = child->requirement();

      if (r.flex_shrink_y) {
        child_box.y_min = y;
        child_box.y_max = y - 1;
        child->SetBox(child_box);
        continue;
      }

      int added_space = extra_space * r.min_y / std::max(1, size);
      extra_space -= added_space;
      size -= r.min_y;

      child_box.y_min = y;
      child_box.y_max = y + r.min_y + added_space - 1;

      child->SetBox(child_box);
      y = child_box.y_max + 1;
    }
  }
};

/// @brief A container displaying elements vertically one by one.
/// @param children The elements in the container
/// @return The container.
/// @ingroup dom
///
/// #### Example
///
/// ```cpp
/// vbox({
///   text(L"Up"),
///   text(L"Down"),
/// });
/// ```
Element vbox(Elements children) {
  return std::make_shared<VBox>(std::move(children));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
