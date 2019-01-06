#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui::dom {

class VBox : public Node {
 public:
  VBox(Children children) : Node(std::move(children)) {}
  ~VBox() {}

  void ComputeRequirement() {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 0;
    requirement_.flex.y = 1;
    for (auto& child : children) {
      child->ComputeRequirement();
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

    int space = box.bottom - box.top + 1;
    int extra_space = space - requirement_.min.y;

    int remaining_flex = flex_sum;
    int remaining_extra_space = extra_space;

    int y = box.top;
    for (auto& child : children) {
      if (y > box.bottom)
        break;

      Box child_box = box;
      child_box.top = y;

      child_box.bottom = y + child->requirement().min.y - 1;

      if (child->requirement().flex.y && remaining_extra_space > 0) {
        int added_space = remaining_extra_space * child->requirement().flex.y /
                          remaining_flex;
        remaining_extra_space -= added_space;
        remaining_flex -= child->requirement().flex.y;
        child_box.bottom += added_space;
      }
      child_box.bottom = std::min(child_box.bottom, box.bottom);

      child->SetBox(child_box);
      y = child_box.bottom + 1;
    }
  }
};

std::unique_ptr<Node> vbox(Children children) {
  return std::make_unique<VBox>(std::move(children));
}

};  // namespace ftxui::dom
