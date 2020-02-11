#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

void NodeDecorator::ComputeRequirement() {
  Node::ComputeRequirement();
  requirement_ = children[0]->requirement();
}

void NodeDecorator::SetBox(Box box) {
  Node::SetBox(box);
  children[0]->SetBox(box);
}

}  // namespace ftxui
