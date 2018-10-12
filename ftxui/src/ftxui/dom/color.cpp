#include "ftxui/dom/node_decorator.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

class BgColor : public NodeDecorator {
 public:
  BgColor(Children children, Color color)
      : NodeDecorator(std::move(children)), color_(color) {}

  void Render(Screen& screen) override {
    NodeDecorator::Render(screen);
    for (int y = box_.top; y <= box_.bottom; ++y) {
      for (int x = box_.left; x <= box_.right; ++x) {
        screen.PixelAt(x, y).background_color = color_;
      }
    }
  }

  Color color_;
};

class FgColor : public NodeDecorator {
 public:
  FgColor(Children children, Color color)
      : NodeDecorator(std::move(children)), color_(color) {}
  ~FgColor() override {}

  void Render(Screen& screen) override {
    NodeDecorator::Render(screen);
    for (int y = box_.top; y <= box_.bottom; ++y) {
      for (int x = box_.left; x <= box_.right; ++x) {
        screen.PixelAt(x, y).foreground_color = color_;
      }
    }
  }

  Color color_;
};

std::unique_ptr<Node> color(Color c, Child child) {
  return std::make_unique<FgColor>(unpack(std::move(child)), c);
}

std::unique_ptr<Node> bgcolor(Color c, Child child) {
  return std::make_unique<BgColor>(unpack(std::move(child)), c);
}

};  // namespace dom
};  // namespace ftxui
