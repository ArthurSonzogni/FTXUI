#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class Flex : public Node {
 public:
  Flex() {}
  Flex(Element child) : Node(unpack(std::move(child))) {}
  ~Flex() override {}
  void ComputeRequirement() override {
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

class NotFlex : public Flex {
 public:
  NotFlex() {}
  NotFlex(Element child) : Flex(std::move(child)) {}
  ~NotFlex() override {}
  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    if (!children.empty()) {
      children[0]->ComputeRequirement();
      requirement_ = children[0]->requirement();
    }
    requirement_.flex.x = 0;
    requirement_.flex.y = 0;
  }
};

std::unique_ptr<Node> filler() {
  return std::make_unique<Flex>();
}

std::unique_ptr<Node> flex(Element child) {
  return std::make_unique<Flex>(std::move(child));
}

std::unique_ptr<Node> notflex(Element child) {
  return std::make_unique<NotFlex>(std::move(child));
}

}  // namespace ftxui
