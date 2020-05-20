// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class DBox : public Node {
 public:
  DBox(Elements children) : Node(std::move(children)) {}
  ~DBox() {}

  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 1;
    requirement_.flex.y = 0;
    for (auto& child : children) {
      child->ComputeRequirement();
      requirement_.min.x =
          std::max(requirement_.min.x, child->requirement().min.x);
      requirement_.min.y =
          std::max(requirement_.min.y, child->requirement().min.y);

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

Element dbox(Elements children) {
  return std::make_shared<DBox>(std::move(children));
}

}  // namespace ftxui
