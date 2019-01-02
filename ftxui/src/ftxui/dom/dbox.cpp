#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class DBox : public Node {
 public:
  DBox(Children children) : Node(std::move(children)) {}
  ~DBox() {}

  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 1;
    requirement_.flex.y = 0;
    for (auto& child : children) {
      child->ComputeRequirement();
      requirement_.min.x = std::max(requirement_.min.x, child->requirement().min.x);
      requirement_.min.y = std::max(requirement_.min.y, child->requirement().min.y);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    for (auto& child : children)
      child->SetBox(box);
  }
};

std::unique_ptr<Node> dbox(Children children) {
  return std::make_unique<DBox>(std::move(children));
}

};  // namespace dom
};  // namespace ftxui
