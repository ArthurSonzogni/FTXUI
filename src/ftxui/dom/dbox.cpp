#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class DBox : public Node {
 public:
  DBox(Elements children) : Node(std::move(children)) {}
  ~DBox() {}

  void ComputeRequirement() override {
    requirement_.min_x = 0;
    requirement_.min_y = 0;
    requirement_.flex_grow_x = 0;
    requirement_.flex_grow_y = 0;
    requirement_.flex_shrink_x = 0;
    requirement_.flex_shrink_y = 0;
    for (auto& child : children) {
      child->ComputeRequirement();
      requirement_.min_x =
          std::max(requirement_.min_x, child->requirement().min_x);
      requirement_.min_y =
          std::max(requirement_.min_y, child->requirement().min_y);

      if (requirement_.selection < child->requirement().selection) {
        requirement_.selection = child->requirement().selection;
        requirement_.selected_box = child->requirement().selected_box;
      }
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    for (auto& child : children)
      child->SetBox(box);
  }
};

/// @brief Stack several element on top of each other.
/// @param children The input element.
/// @return The right aligned element.
/// @ingroup dom
Element dbox(Elements children) {
  return std::make_shared<DBox>(std::move(children));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
