#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class HBox : public Node {
 public:
  HBox(Elements children) : Node(std::move(children)) {}
  ~HBox() {}

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
        requirement_.selected_box.x_min += requirement_.min_x;
        requirement_.selected_box.x_max += requirement_.min_x;
      }
      requirement_.min_x += child->requirement().min_x;
      requirement_.min_y =
          std::max(requirement_.min_y, child->requirement().min_y);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    int space = box.x_max - box.x_min + 1;
    int extra_space = space - requirement_.min_x;

    int size = 0;
    int flex_grow_sum = 0;
    int flex_shrink_sum = 0;
    int flex_shrink_size = 0;
    for (auto& child : children) {
      const Requirement& r = child->requirement();
      flex_grow_sum += r.flex_grow_x;
      flex_shrink_sum += r.min_x * r.flex_shrink_x;
      if (r.flex_shrink_x) {
        flex_shrink_size += r.min_x;
      }
      size += r.min_x;
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
    int x = box.x_min;
    for (auto& child : children) {
      Box child_box = box;
      const Requirement& r = child->requirement();

      int added_space =
          extra_space * r.flex_grow_x / std::max(flex_grow_sum, 1);
      extra_space -= added_space;
      flex_grow_sum -= r.flex_grow_x;

      child_box.x_min = x;
      child_box.x_max = x + r.min_x + added_space - 1;

      child->SetBox(child_box);
      x = child_box.x_max + 1;
    }
  }

  void SetBoxShrinkEasy(Box box, int extra_space, int flex_shrink_sum) {
    int x = box.x_min;
    for (auto& child : children) {
      Box child_box = box;
      const Requirement& r = child->requirement();

      int added_space = extra_space * r.min_x * r.flex_shrink_x /
                        std::max(flex_shrink_sum, 1);
      extra_space -= added_space;
      flex_shrink_sum -= r.flex_shrink_x * r.min_x;

      child_box.x_min = x;
      child_box.x_max = x + r.min_x + added_space - 1;

      child->SetBox(child_box);
      x = child_box.x_max + 1;
    }
  }

  void SetBoxShrinkHard(Box box, int extra_space, int size) {
    int x = box.x_min;
    for (auto& child : children) {
      Box child_box = box;
      const Requirement& r = child->requirement();

      if (r.flex_shrink_x) {
        child_box.x_min = x;
        child_box.x_max = x - 1;
        child->SetBox(child_box);
        continue;
      }

      int added_space = extra_space * r.min_x / std::max(1, size);
      extra_space -= added_space;
      size -= r.min_x;

      child_box.x_min = x;
      child_box.x_max = x + r.min_x + added_space - 1;

      child->SetBox(child_box);
      x = child_box.x_max + 1;
    }
  }
};

/// @brief A container displaying elements horizontally one by one.
/// @param children The elements in the container
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// hbox({
///   text(L"Left"),
///   text(L"Right"),
/// });
/// ```
Element hbox(Elements children) {
  return std::make_shared<HBox>(std::move(children));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
