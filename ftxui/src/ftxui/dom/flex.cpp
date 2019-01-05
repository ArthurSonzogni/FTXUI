#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class Flex : public Node {
 public:
  Flex() {}
  Flex(Element child) : Node(unpack(std::move(child))) {}
  ~Flex() override {}
  void ComputeRequirement() {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    if (!children.empty()) {
      children[0]->ComputeRequirement();
      requirement_ = children[0]->requirement();
    }
    requirement_.flex.x = 1;
    requirement_.flex.y = 1;
  }

  void SetBox(Box box) override {
    if (children.empty())
      return;
    children[0]->SetBox(box);
  }
};

std::unique_ptr<Node> filler() {
  return std::make_unique<Flex>();
}

std::unique_ptr<Node> flex(Element child) {
  return std::make_unique<Flex>(std::move(child));
}

};  // namespace dom
};  // namespace ftxui
