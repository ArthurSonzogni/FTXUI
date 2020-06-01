// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <algorithm>
#include <iostream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class VBox : public Node {
 public:
  VBox(Elements children) : Node(std::move(children)) {}
  ~VBox() {}

  void ComputeRequirement() {
    requirement_.min_x = 0;
    requirement_.min_y = 0;
    requirement_.flex_x = 0;
    requirement_.flex_y = 1;
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

  void SetBox(Box box) {
    Node::SetBox(box);

    int flex_sum = 0;
    for (auto& child : children)
      flex_sum += child->requirement().flex_y;

    int space = box.y_max - box.y_min + 1;
    int extra_space = space - requirement_.min_y;

    int remaining_flex = flex_sum;
    int remaining_extra_space = extra_space;

    int y = box.y_min;
    for (auto& child : children) {
      Box child_box = box;
      child_box.y_min = y;

      child_box.y_max = y + child->requirement().min_y - 1;

      if (child->requirement().flex_y) {
        int added_space = remaining_extra_space * child->requirement().flex_y /
                          remaining_flex;
        remaining_extra_space -= added_space;
        remaining_flex -= child->requirement().flex_y;
        child_box.y_max += added_space;
      }
      child_box.y_max = std::min(child_box.y_max, box.y_max);

      child->SetBox(child_box);
      y = child_box.y_max + 1;
    }
  }
};

Element vbox(Elements children) {
  return std::make_shared<VBox>(std::move(children));
}

}  // namespace ftxui
