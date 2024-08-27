#include "ftxui/dom/elements.hpp"  // for Element, Decorator
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/component/event.hpp"           // for Event


namespace ftxui {
namespace {

class Selectable : public NodeDecorator {
 public:
  explicit Selectable(Element child)
      : NodeDecorator(std::move(child)) {}

 private:
  void Render(Screen& screen) override {

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
        for (int x = box_.x_min; x <= box_.x_max; ++x) {
            screen.PixelAt(x, y).selectable = true;
        }
    }

    NodeDecorator::Render(screen);
  }
};

}  // namespace


Element selectable(Element child) {
  return std::make_shared<Selectable>(std::move(child));
}

Decorator selectable(void) {
  return
      [](Element child) { return selectable(std::move(child)); };
}

}  // namespace ftxui
