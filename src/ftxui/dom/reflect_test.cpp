// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <functional>
#include <vector>
#include <string>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"

// NOLINTBEGIN
namespace ftxui {

TEST(ReflectTest, CallbackBasic) {
  Box reflected_box;
  int callback_count = 0;

  auto element = text("hello") | reflect([&](Box box) {
    reflected_box = box;
    callback_count++;
  }) | size(WIDTH, EQUAL, 5) | size(HEIGHT, EQUAL, 1);

  Screen screen(10, 5);
  Render(screen, element);

  EXPECT_EQ(callback_count, 1);
  EXPECT_EQ(reflected_box.x_min, 0);
  EXPECT_EQ(reflected_box.y_min, 0);
  EXPECT_EQ(reflected_box.x_max, 6); // size(WIDTH, EQUAL, 5) results in box.x_max = 6
  EXPECT_EQ(reflected_box.y_max, 2); // size(HEIGHT, EQUAL, 1) results in box.y_max = 2
}

TEST(ReflectTest, CallbackDeferred) {
  bool callback_executed = false;
  std::vector<std::string> order;

  class CustomElement : public Node {
   public:
    CustomElement(std::vector<std::string>& order) : order_(order) {}
    void Render(Screen& screen) final {
      order_.push_back("render");
      Node::Render(screen);
    }
   private:
    std::vector<std::string>& order_;
  };

  Element child = std::make_shared<CustomElement>(order);
  auto element = child | reflect([&](Box box) {
    order.push_back("callback");
    callback_executed = true;
  });

  Screen screen(5, 5);
  Render(screen, element);

  EXPECT_TRUE(callback_executed);
  ASSERT_EQ(order.size(), 2);
  EXPECT_EQ(order[0], "render");
  EXPECT_EQ(order[1], "callback");
}

}  // namespace ftxui
// NOLINTEND
