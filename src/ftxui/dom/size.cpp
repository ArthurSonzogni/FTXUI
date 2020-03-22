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

    auto& value = direction_ == WIDTH ? requirement_.min.x : requirement_.min.y;

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

    if (direction_ == WIDTH)
      requirement_.flex.x = 0;
    else
      requirement_.flex.y = 0;
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

Decorator size(Direction direction, Constraint constraint, int value) {
  return [=](Element e) {
    return std::make_unique<Size>(std::move(e), direction, constraint, value);
  };
}

}  // namespace ftxui
