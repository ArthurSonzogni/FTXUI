#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node_decorator.hpp"

namespace ftxui {

class BgColor : public NodeDecorator {
 public:
  BgColor(Elements children, Color color)
      : NodeDecorator(std::move(children)), color_(color) {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).background_color = color_;
      }
    }
    NodeDecorator::Render(screen);
  }

  Color color_;
};

class FgColor : public NodeDecorator {
 public:
  FgColor(Elements children, Color color)
      : NodeDecorator(std::move(children)), color_(color) {}
  ~FgColor() override {}

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.PixelAt(x, y).foreground_color = color_;
      }
    }
    NodeDecorator::Render(screen);
  }

  Color color_;
};

std::unique_ptr<Node> color(Color c, Element child) {
  return std::make_unique<FgColor>(unpack(std::move(child)), c);
}

std::unique_ptr<Node> bgcolor(Color c, Element child) {
  return std::make_unique<BgColor>(unpack(std::move(child)), c);
}

Decorator color(Color c) {
  return [c](Element child) { return color(c, std::move(child)); };
}

Decorator bgcolor(Color c) {
  return [c](Element child) { return bgcolor(c, std::move(child)); };
}

}  // namespace ftxui
