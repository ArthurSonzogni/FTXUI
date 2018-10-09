#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class HBox : public Node {
 public:
  HBox(Children children) : Node(std::move(children)) {}
  ~HBox() {}

  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 1;
    requirement_.flex.y = 0;
    for (auto& child : children) {
      child->ComputeRequirement();
      requirement_.min.x += child->requirement().min.x;
      requirement_.min.y =
          std::max(requirement_.min.y, child->requirement().min.y);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    int flex_sum = 0;
    for (auto& child : children)
      flex_sum += child->requirement().flex.x;

    int space = box.right - box.left + 1;
    int extra_space = space - requirement_.min.x;

    int remaining_flex = flex_sum;
    int remaining_extra_space = extra_space;

    int x = box.left;
    for (auto& child : children) {
      if (x > box.right)
        break;

      Box child_box = box;
      child_box.left = x;

      child_box.right = x + child->requirement().min.x - 1;

      if (child->requirement().flex.x && remaining_extra_space > 0) {
        int added_space = remaining_extra_space * child->requirement().flex.x /
                          remaining_flex;
        remaining_extra_space -= added_space;
        remaining_flex -= child->requirement().flex.x;
        child_box.right += added_space;
      }
      child_box.right = std::min(child_box.right, box.right);

      child->SetBox(child_box);
      x = child_box.right + 1;
    }
  }
};

std::unique_ptr<Node> hbox(Children children) {
  return std::make_unique<HBox>(std::move(children));
}

};  // namespace dom
};  // namespace ftxui
