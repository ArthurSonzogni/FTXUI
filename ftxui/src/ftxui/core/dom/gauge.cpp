#include "ftxui/core/dom/node.hpp"
#include "ftxui/core/dom/elements.hpp"

namespace ftxui {
namespace dom {

class Gauge : public Node {
 public:
  Gauge(float progress) : progress_(progress) {}
  ~Gauge() {}

  void ComputeRequirement() override {
    requirement_.flex.x = 1;
    requirement_.min.y = 1;
  }

  void Render(Screen& screen) override {
    float y = box_.top;
    int limit = box_.left + progress_ * (box_.right - box_.left);
    for(int i = box_.left; i<=limit; ++i)
      screen.at(i, y) = 'X';
  }
 private:
  float progress_;
};

std::unique_ptr<Node> gauge(float progress) {
  return std::make_unique<Gauge>(progress);
}

};  // namespace dom
};  // namespace ftxui
