#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {
namespace dom {

void NodeDecorator::ComputeRequirement() {
  Node::ComputeRequirement();
  requirement_ = children[0]->requirement();
}

void NodeDecorator::SetBox(Box box) {
  Node::SetBox(box);
  children[0]->SetBox(box);
}

};  // namespace dom
};  // namespace ftxui
