#include "ftxui/dom/node_decorator.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

class Bold : public NodeDecorator {
 public:
  Bold(Children children) : NodeDecorator(std::move(children)) {}
  ~Bold() override {}

  void Render(Screen& screen) override {
    for (int y = box_.top; y <= box_.bottom; ++y) {
      for (int x = box_.left; x <= box_.right; ++x) {
        screen.PixelAt(x,y).bold = true; 
      }
    }
    Node::Render(screen);
  }
};

std::unique_ptr<Node> bold(Child child) {
  return std::make_unique<Bold>(unpack(std::move(child)));
}

};  // namespace ftxui
