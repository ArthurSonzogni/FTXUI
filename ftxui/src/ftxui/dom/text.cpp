#include "ftxui/dom/node.hpp"

namespace ftxui {

using ftxui::Screen;

class Text : public Node {
 public:
  Text(std::wstring text) : Node(), text_(text) {}
  ~Text() {}

  void ComputeRequirement() override {
    requirement_.min.x = text_.size();
    requirement_.min.y = 1;
  }

  void Render(Screen& screen) override {
    int x = box_.x_min;
    int y = box_.y_min;
    if (y > box_.y_max)
      return;
    for (wchar_t c : text_) {
      if (x > box_.x_max)
        return;
      screen.at(x++, y) = c;
    }
  }

 private:
  std::wstring text_;
};

std::unique_ptr<Node> text(std::wstring text) {
  return std::make_unique<Text>(text);
}

};  // namespace ftxui
