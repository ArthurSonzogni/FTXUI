#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

using ftxui::Screen;

class Inverted : public NodeDecorator {
 public:
  Inverted(Elements children) : NodeDecorator(std::move(children)) {}
  ~Inverted() override {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).inverted = true;
      }
    }
  }
};

std::unique_ptr<Node> inverted(Element child) {
  return std::make_unique<Inverted>(unpack(std::move(child)));
}

}  // namespace ftxui
