#include "ftxui/dom/node_decorator.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class Inverted : public NodeDecorator {
 public:
  Inverted(Children children) : NodeDecorator(std::move(children)) {}
  ~Inverted() override {}

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
