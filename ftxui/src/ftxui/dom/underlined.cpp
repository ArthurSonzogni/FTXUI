#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class Underlined : public Node {
 public:
  Underlined(Children children) : Node(std::move(children)) {}
  ~Underlined() override {}

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
        screen.PixelAt(x,y).underlined = true; 
      }
    }
  }
};

std::unique_ptr<Node> underlined(Child child) {
  return std::make_unique<Underlined>(unpack(std::move(child)));
}

};  // namespace dom
};  // namespace ftxui
