#include "ftxui/dom/node.hpp"

namespace ftxui::dom {

using ftxui::screen::Screen;

Node::Node() {}
Node::Node(std::vector<std::unique_ptr<Node>> children)
    : children(std::move(children)) {}
Node::~Node() {}

void Node::ComputeRequirement() {
  for(auto& child : children)
    child->ComputeRequirement();
}

void Node::SetBox(Box box) {
  box_ = box;
}

void Node::Render(Screen& screen) {
  for(auto& child : children)
    child->Render(screen);
}

void Render(Screen& screen, Node* node) {
  // Step 1: Find what dimension this elements wants to be.
  node->ComputeRequirement();
  
  Box box;
  box.left = 0;
  box.top = 0;
  box.right = screen.dimx() - 1;
  box.bottom = screen.dimy() - 1;
  
  // Step 2: Assign a dimension to the element.
  node->SetBox(box);

  // Step 3: Draw the element.
  node->Render(screen);
}

};  // namespace ftxui::dom
