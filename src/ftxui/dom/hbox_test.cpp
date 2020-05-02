// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"

using namespace ftxui;
using namespace ftxui;

TEST(HBoxTest, ScreenSmaller1) {
  auto root = hbox(text(L"text_1"), text(L"text_2"));
  Screen screen(11, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1text_", screen.ToString());
}

TEST(HBoxTest, ScreenSmaller2) {
  auto root = hbox(text(L"text_1"), text(L"text_2"));
  Screen screen(10, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1text", screen.ToString());
}

TEST(HBoxTest, ScreenFit) {
  auto root = hbox(text(L"text_1"), text(L"text_2"));
  Screen screen(12, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1text_2", screen.ToString());
}

TEST(HBoxTest, ScreenBigger1) {
  auto root = hbox(text(L"text_1"), text(L"text_2"));
  Screen screen(13, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1text_2 ", screen.ToString());
}
TEST(HBoxTest, ScreenBigger2) {
  auto root = hbox(text(L"text_1"), text(L"text_2"));
  Screen screen(14, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1text_2  ", screen.ToString());
}

TEST(HBoxTest, ScreenSmaller1Flex) {
  auto root = hbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(11, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_text_2", screen.ToString());
}

TEST(HBoxTest, ScreenSmaller2Flex) {
  auto root = hbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(10, 1);
  Render(screen, root.get());

  EXPECT_EQ("texttext_2", screen.ToString());
}

TEST(HBoxTest, ScreenFitFlex) {
  auto root = hbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(12, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1text_2", screen.ToString());
}

TEST(HBoxTest, ScreenBigger1Flex) {
  auto root = hbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(13, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1 text_2", screen.ToString());
}

TEST(HBoxTest, ScreenBigger2Flex) {
  auto root = hbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(14, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1  text_2", screen.ToString());
}
