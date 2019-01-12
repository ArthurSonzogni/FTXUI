#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

class Size : public Node {
 public:
  Size(Element child, size_t width, size_t height)
      : Node(unpack(std::move(child))), width_(width), height_(height) {}
  ~Size() override {}
  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_.min.x = width_;
    requirement_.min.y = height_;
    requirement_.flex.x = 0;
    requirement_.flex.y = 0;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    children[0]->SetBox(box);
  }

 private:
  size_t width_;
  size_t height_;
};

Decorator size(size_t width, size_t height) {
  return [=](Element e) {
    return std::make_unique<Size>(std::move(e), width, height);
  };
}

};  // namespace ftxui
