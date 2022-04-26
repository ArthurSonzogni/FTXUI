#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFactoryImpl, TestPartResult
#include "gtest/gtest_pred_impl.h"       // for Test, EXPECT_EQ, TEST
#include <string>                   // for allocator
#include "ftxui/dom/elements.hpp"        // for text, flexbox
#include "ftxui/screen/screen.hpp"       // for Screen

namespace ftxui {

namespace {
int Hash(const std::string s) {
  int hash = 0;
  for (auto c : s) {
    hash += c;
  }
  return hash;
}
}

TEST(BorderTest, GoldPoint) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  auto element = canvas([](Canvas& c) {  //
    c.DrawPoint(3, 3, 1, Color::Red);
    c.DrawPointToggle(2, 8);
    c.DrawPointLine(3, 7, 10, 19, Color::Blue);
    c.DrawPointCircle(10, 5, 3, Color::Yellow);
    c.DrawPointCircleFilled(20, 5, 3, Color::Green);
    c.DrawPointEllipse(10, 10, 5, 2, Color::Blue);
    c.DrawPointEllipseFilled(10, 20, 5, 2, Color::DarkGreen);
  });
  Screen screen(30, 10);
  Render(screen, element);
  EXPECT_EQ(Hash(screen.ToString()), 17651);
}

TEST(BorderTest, GoldBlock) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  auto element = canvas([](Canvas& c) {  //
    c.DrawBlock(3, 3, 1, Color::Red);
    c.DrawBlockToggle(2, 8);
    c.DrawBlockLine(3, 7, 10, 19, Color::Blue);
    c.DrawBlockCircle(10, 5, 3, Color::Yellow);
    c.DrawBlockCircleFilled(20, 5, 3, Color::Green);
    c.DrawBlockEllipse(10, 10, 5, 2, Color::Blue);
    c.DrawBlockEllipseFilled(10, 20, 5, 2, Color::DarkGreen);
  });
  Screen screen(30, 10);
  Render(screen, element);
  EXPECT_EQ(Hash(screen.ToString()), 14383);
}

} // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
