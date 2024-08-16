// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>   // for make_shared, __shared_ptr_access
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"     // for Element, unpack, Decorator, reflect
#include "ftxui/dom/node.hpp"         // for Node, Elements
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Screen

namespace ftxui {
namespace {

// Helper class.
class Reflect : public Node {
 public:
  Reflect(Element child, Box& box)
      : Node(unpack(std::move(child))), reflected_box_(box) {}

  void ComputeRequirement() final {
    Node::ComputeRequirement();
    requirement_ = children_[0]->requirement();
  }

  void SetBox(Box box) final {
    reflected_box_ = box;
    Node::SetBox(box);
    children_[0]->SetBox(box);
  }

  void Render(Screen& screen) final {
    reflected_box_ = Box::Intersection(screen.stencil, reflected_box_);
    Node::Render(screen);
  }

 private:
  Box& reflected_box_;
};
}  // namespace

Decorator reflect(Box& box) {
  return [&](Element child) -> Element {
    return std::make_shared<Reflect>(std::move(child), box);
  };
}

}  // namespace ftxui
