#include "ftxui/dom/node.hpp"

namespace ftxui {

using ftxui::Screen;

Node::Node() {}
Node::Node(Elements children) : children(std::move(children)) {}
Node::~Node() {}

/// @brief Compute how much space an elements needs.
/// @ingroup dom
void Node::ComputeRequirement() {
  for (auto& child : children)
    child->ComputeRequirement();
}

/// @brief Assign a position and a dimension to an element for drawing.
/// @ingroup dom
void Node::SetBox(Box box) {
  box_ = box;
}

/// @brief Display an element on a ftxui::Screen.
/// @ingroup dom
void Node::Render(Screen& screen) {
  for (auto& child : children)
    child->Render(screen);
}

/// @brief Display an element on a ftxui::Screen.
/// @ingroup dom
void Render(Screen& screen, const Element& element) {
  Render(screen, element.get());
}

/// @brief Display an element on a ftxui::Screen.
/// @ingroup dom
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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
