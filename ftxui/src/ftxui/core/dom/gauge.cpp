#include "ftxui/core/dom/node.hpp"
#include "ftxui/core/dom/elements.hpp"

namespace ftxui {
namespace dom {

static wchar_t charset[] = L"  ▏▎▍▌▋▊▉█";

class Gauge : public Node {
 public:
  Gauge(float progress) : progress_(progress) {}
  ~Gauge() override {}

  void ComputeRequirement() override {
    requirement_.flex.x = 1;
    requirement_.min.y = 1;
  }

  void Render(Screen& screen) override {
    float y = box_.top;
    float limit = box_.left + progress_ * (box_.right - box_.left + 1);
    int limit_int = limit;
    int x = box_.left;
    while (x < limit_int)
      screen.at(x++, y) = charset[9];
    screen.at(x++, y) = charset[int(9*(limit-limit_int))];
    while (x <= box_.right)
      screen.at(x++, y) = charset[0];
  }
 private:
  float progress_;
};

std::unique_ptr<Node> gauge(float progress) {
  return std::make_unique<Gauge>(progress);
}

};  // namespace dom
};  // namespace ftxui
