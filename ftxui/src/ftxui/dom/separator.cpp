#include "ftxui/dom/node.hpp"

namespace ftxui {

using ftxui::Screen;

class Separator : public Node {
 public:
  Separator() {}
  ~Separator() override {}
  void ComputeRequirement() override {
    requirement_.min.x = 1;
    requirement_.min.y = 1;
  }

  void Render(Screen& screen) override {
    bool is_column = (box_.x_max == box_.x_min);
    bool is_line = (box_.y_min == box_.y_max);

    wchar_t c = U'+';
    if (is_line && !is_column)
      c = U'─';
    else
      c = U'│';

    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        screen.at(x, y) = c;
      }
    }
  }
};

std::unique_ptr<Node> separator() {
  return std::make_unique<Separator>();
}

};  // namespace ftxui
