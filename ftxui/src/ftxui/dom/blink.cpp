#include "ftxui/dom/node_decorator.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class Blink : public NodeDecorator {
 public:
  Blink(Children children) : NodeDecorator(std::move(children)) {}
  ~Blink() override {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.top; y <= box_.bottom; ++y) {
      for (int x = box_.left; x <= box_.right; ++x) {
        screen.PixelAt(x, y).blink = true;
      }
    }
  }
};

std::unique_ptr<Node> blink(Child child) {
  return std::make_unique<Blink>(unpack(std::move(child)));
}

};  // namespace dom
};  // namespace ftxui
