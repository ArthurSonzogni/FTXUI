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
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 0;
    requirement_.flex.y = 1;
    for (auto& child : children) {
      child->ComputeRequirement();
      if (requirement_.selection < child->requirement().selection) {
        requirement_.selection = child->requirement().selection;
        requirement_.selected_box = child->requirement().selected_box;
        requirement_.selected_box.y_min += requirement_.min.y;
        requirement_.selected_box.y_max += requirement_.min.y;
      }
      requirement_.min.y += child->requirement().min.y;
      requirement_.min.x =
          std::max(requirement_.min.x, child->requirement().min.x);
    }
  }

  void SetBox(Box box) {
    Node::SetBox(box);

    int flex_sum = 0;
    for (auto& child : children)
      flex_sum += child->requirement().flex.y;

    int space = box.y_max - box.y_min + 1;
    int extra_space = space - requirement_.min.y;

    int remaining_flex = flex_sum;
    int remaining_extra_space = extra_space;

    int y = box.y_min;
    for (auto& child : children) {
      Box child_box = box;
      child_box.y_min = y;

      child_box.y_max = y + child->requirement().min.y - 1;

      if (child->requirement().flex.y && remaining_extra_space > 0) {
        int added_space = remaining_extra_space * child->requirement().flex.y /
                          remaining_flex;
        remaining_extra_space -= added_space;
        remaining_flex -= child->requirement().flex.y;
        child_box.y_max += added_space;
      }
      child_box.y_max = std::min(child_box.y_max, box.y_max);

      child->SetBox(child_box);
      y = child_box.y_max + 1;
    }
  }
};

std::unique_ptr<Node> vbox(Elements children) {
  return std::make_unique<VBox>(std::move(children));
}

}  // namespace ftxui
