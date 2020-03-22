#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

class Bold : public NodeDecorator {
 public:
  Bold(Elements children) : NodeDecorator(std::move(children)) {}
  ~Bold() override {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).bold = true;
      }
    }
    Node::Render(screen);
  }
};

std::unique_ptr<Node> bold(Element child) {
  return std::make_unique<Bold>(unpack(std::move(child)));
}

}  // namespace ftxui
