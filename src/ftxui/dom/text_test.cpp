#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult
#include <memory>                   // for allocator

#include "ftxui/dom/elements.hpp"   // for text, Element, operator|, border
#include "ftxui/screen/box.hpp"     // for ftxui
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest_pred_impl.h"  // for Test, SuiteApiResolver, EXPECT_EQ

using namespace ftxui;

TEST(TextTest, ScreenHeightSmaller) {
  auto element = text(L"test");
  Screen screen(2, 0);
  Render(screen, element);

  EXPECT_EQ("", screen.ToString());
}

TEST(TextTest, ScreenSmaller) {
  auto element = text(L"test");
  Screen screen(2, 1);
  Render(screen, element);

  EXPECT_EQ("te", screen.ToString());
}

TEST(TextTest, ScreenFit) {
  auto element = text(L"test");
  Screen screen(4, 1);
  Render(screen, element);

  EXPECT_EQ("test", screen.ToString());
}

TEST(TextTest, ScreenBigger) {
  auto element = text(L"test");
  Screen screen(6, 1);
  Render(screen, element);

  EXPECT_EQ("test  ", screen.ToString());
}

TEST(TextTest, ScreenBigger2) {
  auto element = text(L"test");
  Screen screen(6, 2);
  Render(screen, element);

  EXPECT_EQ("test  \r\n      ", screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK) {
  auto element = text(L"测试") | border;
  Screen screen(6, 3);
  Render(screen, element);
  EXPECT_EQ(
      "┌────┐\r\n"
      "│测试│\r\n"
      "└────┘",
      screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK_2) {
  auto element = text(L"测试") | border;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(
      "┌───┐\r\n"
      "│测试\r\n"
      "└───┘",
      screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK_3) {
  auto element = text(L"测试") | border;
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(
      "┌──┐\r\n"
      "│测│\r\n"
      "└──┘",
      screen.ToString());
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
