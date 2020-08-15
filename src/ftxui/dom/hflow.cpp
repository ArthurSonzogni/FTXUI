#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class HFlow : public Node {
 public:
  HFlow(Elements children) : Node(std::move(children)) {}
  ~HFlow() {}

  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 1;
    requirement_.flex_shrink_x = 0;
    requirement_.flex_shrink_y = 0;
    for (auto& child : children)
      child->ComputeRequirement();
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    // The position of the first component.
    int x = box.x_min;
    int y = box.y_min;
    int y_next = y;  // The position of next row of elements.

    for (auto& child : children) {
      Requirement requirement = child->requirement();

      // Does it fit the end of the row?
      if (x + requirement.min_x > box.x_max) {
        // No? Use the next row.
        x = box.x_min;
        y = y_next;
      }

      // Does the current row big enough to contain the element?
      if (y + requirement.min_y > box.y_max + 1)
        break;  // No? Ignore the element.

      Box children_box;
      children_box.x_min = x;
      children_box.x_max = x + requirement.min_x - 1;
      children_box.y_min = y;
      children_box.y_max = y + requirement.min_y - 1;
      child->SetBox(children_box);

      x = x + requirement.min_x;
      y_next = std::max(y_next, y + requirement.min_y);
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
Element hflow(Elements children) {
  return std::make_shared<HFlow>(std::move(children));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
