#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

class Blink : public NodeDecorator {
 public:
  Blink(Elements children) : NodeDecorator(std::move(children)) {}
  ~Blink() override {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).blink = true;
      }
    }
  }
};

std::unique_ptr<Node> blink(Element child) {
  return std::make_unique<Blink>(unpack(std::move(child)));
}

}  // namespace ftxui
