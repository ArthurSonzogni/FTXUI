#include "ftxui/dom/node.hpp"

namespace ftxui {

using ftxui::Screen;

Node::Node() {}
Node::Node(std::vector<std::unique_ptr<Node>> children)
    : children(std::move(children)) {}
Node::~Node() {}

void Node::ComputeRequirement() {
  for (auto& child : children)
    child->ComputeRequirement();
}

void Node::SetBox(Box box) {
  box_ = box;
}

void Node::Render(Screen& screen) {
  for (auto& child : children)
    child->Render(screen);
}

void Render(Screen& screen, Node* node) {
  // Step 1: Find what dimension this elements wants to be.
  node->ComputeRequirement();

  Box box;
  box.x_min = 0;
  box.y_min = 0;
  box.x_max = screen.dimx() - 1;
  box.y_max = screen.dimy() - 1;

  // Step 2: Assign a dimension to the element.
  node->SetBox(box);
  screen.stencil = box;

  // Step 3: Draw the element.
  node->Render(screen);

  // Step 4: Apply shaders
  screen.ApplyShader();
}

}  // namespace ftxui
