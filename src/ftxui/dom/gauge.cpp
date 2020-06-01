// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

static wchar_t charset[] = L"  ▏▎▍▌▋▊▉█";

class Gauge : public Node {
 public:
  Gauge(float progress) : progress_(progress) {}
  ~Gauge() override {}

  void ComputeRequirement() override {
    requirement_.flex_x = 1;
    requirement_.min_y = 1;
  }

  void Render(Screen& screen) override {
    float y = box_.y_min;
    float limit = box_.x_min + progress_ * (box_.x_max - box_.x_min + 1);
    int limit_int = limit;
    int x = box_.x_min;
    while (x < limit_int)
      screen.at(x++, y) = charset[9];
    screen.at(x++, y) = charset[int(9 * (limit - limit_int))];
    while (x <= box_.x_max)
      screen.at(x++, y) = charset[0];
  }

 private:
  float progress_;
};

Element gauge(float progress) {
  return std::make_shared<Gauge>(progress);
}

}  // namespace ftxui
