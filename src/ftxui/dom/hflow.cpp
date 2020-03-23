#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class HFlow : public Node {
 public:
  HFlow(Elements children) : Node(std::move(children)) {}
  ~HFlow() {}

  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 1;
    requirement_.flex.y = 1;
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
      if (x + requirement.min.x > box.x_max) {
        // No? Use the next row.
        x = box.x_min;
        y = y_next;
      }

      // Does the current row big enough to contain the element?
      if (y + requirement.min.y > box.y_max + 1)
        break;  // No? Ignore the element.

      Box children_box;
      children_box.x_min = x;
      children_box.x_max = x + requirement.min.x - 1;
      children_box.y_min = y;
      children_box.y_max = y + requirement.min.y - 1;
      child->SetBox(children_box);

      x = x + requirement.min.x;
      y_next = std::max(y_next, y + requirement.min.y);
    }
  }
};

std::unique_ptr<Node> hflow(Elements children) {
  return std::make_unique<HFlow>(std::move(children));
}

}  // namespace ftxui
