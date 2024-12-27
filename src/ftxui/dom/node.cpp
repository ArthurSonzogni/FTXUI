#include <iostream>
// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/screen/box.hpp>  // for Box
#include <utility>               // for move

#include "ftxui/dom/node.hpp"
#include "ftxui/screen/screen.hpp"  // for Screen

namespace ftxui {

Node::Node() = default;
Node::Node(Elements children) : children_(std::move(children)) {}
Node::~Node() = default;

/// @brief Compute how much space an elements needs.
/// @ingroup dom
void Node::ComputeRequirement() {
  for (auto& child : children_) {
    child->ComputeRequirement();
  }
}

/// @brief Assign a position and a dimension to an element for drawing.
/// @ingroup dom
void Node::SetBox(Box box) {
  box_ = box;
}

/// @brief Compute the selection of an element.
/// @ingroup dom
void Node::Select(Selection& selection) {
  // If this Node box_ doesn't intersect with the selection, then no selection.
  if (Box::Intersection(selection.GetBox(), box_).IsEmpty()) {
    return;
  }

  // By default we defer the selection to the children.
  for (auto& child : children_) {
    child->Select(selection);
  }
}

/// @brief Display an element on a ftxui::Screen.
/// @ingroup dom
void Node::Render(Screen& screen) {
  for (auto& child : children_) {
    child->Render(screen);
  }
}

void Node::Check(Status* status) {
  for (auto& child : children_) {
    child->Check(status);
  }
  status->need_iteration |= (status->iteration == 0);
}

std::string Node::GetSelectedContent(Selection& selection) {
  std::string content;

  for (auto& child : children_) {
    content += child->GetSelectedContent(selection);
  }

  return content;
}

/// @brief Display an element on a ftxui::Screen.
/// @ingroup dom
void Render(Screen& screen, const Element& element) {
  Selection selection;
  Render(screen, element.get(), selection);
}

/// @brief Display an element on a ftxui::Screen.
/// @ingroup dom
void Render(Screen& screen, Node* node) {
  Selection selection;
  Render(screen, node, selection);
}

void Render(Screen& screen, Node* node, Selection& selection) {
  Box box;
  box.x_min = 0;
  box.y_min = 0;
  box.x_max = screen.dimx() - 1;
  box.y_max = screen.dimy() - 1;

  Node::Status status;
  node->Check(&status);
  const int max_iterations = 20;
  while (status.need_iteration && status.iteration < max_iterations) {
    // Step 1: Find what dimension this elements wants to be.
    node->ComputeRequirement();

    // Step 2: Assign a dimension to the element.
    node->SetBox(box);

    // Check if the element needs another iteration of the layout algorithm.
    status.need_iteration = false;
    status.iteration++;
    node->Check(&status);
  }

  // Step 3: Selection
  if (!selection.IsEmpty()) {
    node->Select(selection);
  }

  // Step 4: Draw the element.
  screen.stencil = box;
  node->Render(screen);

  // Step 5: Apply shaders
  screen.ApplyShader();
}

std::string GetNodeSelectedContent(Screen& screen,
                                   Node* node,
                                   Selection& selection) {
  Box box;
  box.x_min = 0;
  box.y_min = 0;
  box.x_max = screen.dimx() - 1;
  box.y_max = screen.dimy() - 1;

  Node::Status status;
  node->Check(&status);
  const int max_iterations = 20;
  while (status.need_iteration && status.iteration < max_iterations) {
    // Step 1: Find what dimension this elements wants to be.
    node->ComputeRequirement();

    // Step 2: Assign a dimension to the element.
    node->SetBox(box);

    // Check if the element needs another iteration of the layout algorithm.
    status.need_iteration = false;
    status.iteration++;
    node->Check(&status);
  }

  // Step 3: Selection
  node->Select(selection);

  // Step 4: get the selected content.
  return node->GetSelectedContent(selection);
}

}  // namespace ftxui
