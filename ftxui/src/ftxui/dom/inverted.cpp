#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class Inverted : public Node {
 public:
  Inverted(Children children) : Node(std::move(children)) {}
  ~Inverted() override {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    children[0]->SetBox(box);
  }

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.top; y <= box_.bottom; ++y) {
      for (int x = box_.left; x <= box_.right; ++x) {
        screen.PixelAt(x,y).inverted = true; 
      }
    }
  }
};

std::unique_ptr<Node> inverted(Child child) {
  return std::make_unique<Inverted>(unpack(std::move(child)));
}

};  // namespace dom
};  // namespace ftxui
