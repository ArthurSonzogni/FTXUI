// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

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
    requirement_.flex_x = 1;
    requirement_.flex_y = 0;
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

    int flex_sum = 0;
    for (auto& child : children)
      flex_sum += child->requirement().flex_x;

    int space = box.x_max - box.x_min + 1;
    int extra_space = space - requirement_.min_x;

    int remaining_flex = flex_sum;
    int remaining_extra_space = extra_space;

    int x = box.x_min;
    for (auto& child : children) {
      Box child_box = box;
      child_box.x_min = x;

      child_box.x_max = x + child->requirement().min_x - 1;

      if (child->requirement().flex_x) {
        int added_space = remaining_extra_space * child->requirement().flex_x /
                          remaining_flex;
        remaining_extra_space -= added_space;
        remaining_flex -= child->requirement().flex_x;
        child_box.x_max += added_space;
      }
      child_box.x_max = std::min(child_box.x_max, box.x_max);

      child->SetBox(child_box);
      x = child_box.x_max + 1;
    }
  }
};

Element hbox(Elements children) {
  return std::make_shared<HBox>(std::move(children));
}

}  // namespace ftxui
