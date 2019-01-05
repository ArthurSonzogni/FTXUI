#include "ftxui/dom/node.hpp"

namespace ftxui {
namespace dom {

class Separator : public Node {
 public:
  Separator() {}
  ~Separator() override {}
  void ComputeRequirement() override {
    requirement_.min.x = 1;
    requirement_.min.y = 1;
  }

  void Render(Screen& screen) override {
    bool is_column = (box_.right == box_.left);
    bool is_line = (box_.top == box_.bottom);

    wchar_t c = U'+';
    if (is_line && !is_column)
      c = U'─';
    else
      c = U'│';

    for (int y = box_.top; y <= box_.bottom; ++y) {
      for (int x = box_.left; x <= box_.right; ++x) {
        screen.at(x, y) = c;
      }
    }
  }
};

std::unique_ptr<Node> separator() {
  return std::make_unique<Separator>();
}

};  // namespace dom
};  // namespace ftxui
