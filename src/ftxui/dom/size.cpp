#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class Size : public Node {
 public:
  Size(Element child, Direction direction, Constraint constraint, size_t value)
      : Node(unpack(std::move(child))),
        direction_(direction),
        constraint_(constraint),
        value_(value) {}

  ~Size() override {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();

    auto& value = direction_ == WIDTH ? requirement_.min_x : requirement_.min_y;

    switch (constraint_) {
      case LESS_THAN:
        value = std::min(value, value_);
        break;
      case EQUAL:
        value = value_;
        break;
      case GREATER_THAN:
        value = std::max(value, value_);
        break;
    }

    if (direction_ == WIDTH) {
      requirement_.flex_grow_x = 0;
      requirement_.flex_shrink_x = 0;
    } else {
      requirement_.flex_grow_y = 0;
      requirement_.flex_shrink_y = 0;
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    if (direction_ == WIDTH) {
      switch (constraint_) {
        case LESS_THAN:
        case EQUAL:
          box.x_max = std::min(box.x_min + value_ + 1, box.x_max);
          break;
        case GREATER_THAN:
          break;
      }
    } else {
      switch (constraint_) {
        case LESS_THAN:
        case EQUAL:
          box.y_max = std::min(box.y_min + value_ + 1, box.y_max);
          break;
        case GREATER_THAN:
          break;
      }
    }
    children[0]->SetBox(box);
  }

 private:
  Direction direction_;
  Constraint constraint_;
  int value_;
};

/// @brief Apply a constraint on the size of an element.
/// @param direction Whether the WIDTH of the HEIGHT of the element must be
///                  constrained.
/// @param constrain The type of constaint.
/// @param value the value.
/// @ingroup dom
Decorator size(Direction direction, Constraint constraint, int value) {
  return [=](Element e) {
    return std::make_shared<Size>(std::move(e), direction, constraint, value);
  };
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
