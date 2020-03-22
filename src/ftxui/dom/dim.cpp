#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

using ftxui::Screen;

class Dim : public NodeDecorator {
 public:
  Dim(Elements children) : NodeDecorator(std::move(children)) {}
  ~Dim() override {}

  void Render(Screen& screen) override {
    Node::Render(screen);
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).dim = true;
      }
    }
  }
};

std::unique_ptr<Node> dim(Element child) {
  return std::make_unique<Dim>(unpack(std::move(child)));
}

}  // namespace ftxui
