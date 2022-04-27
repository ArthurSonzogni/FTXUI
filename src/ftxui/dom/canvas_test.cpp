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
    c.DrawPoint(3, 3, 1);
    c.DrawPointToggle(2, 8);
    c.DrawPointToggle(2, 8);
    c.DrawPointToggle(2, 8);
    c.DrawPointLine(3, 7, 10, 19);
    c.DrawPointCircle(10, 5, 3);
    c.DrawPointCircleFilled(20, 5, 3);
    c.DrawPointEllipse(10, 10, 5, 2);
    c.DrawPointEllipseFilled(10, 20, 5, 2);
  });
  Screen screen(30, 10);
  Render(screen, element);
  EXPECT_EQ(Hash(screen.ToString()), 1069);
}

TEST(BorderTest, GoldBlock) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  auto element = canvas([](Canvas& c) {  //
    c.DrawBlock(3, 3, 1);
    c.DrawBlockToggle(2, 8);
    c.DrawBlockToggle(2, 8);
    c.DrawBlockToggle(2, 8);
    c.DrawBlockLine(3, 7, 10, 19);
    c.DrawBlockCircle(10, 5, 3);
    c.DrawBlockCircleFilled(20, 5, 3);
    c.DrawBlockEllipse(10, 10, 5, 2);
    c.DrawBlockEllipseFilled(10, 20, 5, 2);
  });
  Screen screen(30, 10);
  Render(screen, element);
  EXPECT_EQ(Hash(screen.ToString()), 472);
}

TEST(BorderTest, GoldText) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  Canvas c(10, 10);
  c.DrawText(0, 0, "test");
  c.DrawText(0, 5, "test");
  c.DrawText(0, 10, "test");
  auto element = canvas(c);
  Screen screen(30, 10);
  Render(screen, element);
  EXPECT_EQ(Hash(screen.ToString()), 10447);
}

} // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
