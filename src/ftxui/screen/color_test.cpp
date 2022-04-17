#include "ftxui/screen/color.hpp"
#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include "ftxui/screen/terminal.hpp"
#include "gtest/gtest_pred_impl.h"  // for EXPECT_EQ, Test, TEST

namespace ftxui {

TEST(ColorTest, PrintTransparent) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color().Print(false), "39");
  EXPECT_EQ(Color().Print(true), "49");
}

TEST(ColorTest, PrintColor16) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color(Color::Red).Print(false), "31");
  EXPECT_EQ(Color(Color::Red).Print(true), "41");
}

TEST(ColorTest, PrintColor256) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color(Color::DarkRed).Print(false), "38;5;52");
  EXPECT_EQ(Color(Color::DarkRed).Print(true), "48;5;52");
}

TEST(ColorTest, PrintTrueCOlor) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color::RGB(1,2,3).Print(false), "38;2;1;2;3");
  EXPECT_EQ(Color::RGB(1,2,3).Print(true), "48;2;1;2;3");
}

TEST(ColorTest, FallbackTo256) {
  Terminal::SetColorSupport(Terminal::Color::Palette256);
  EXPECT_EQ(Color::RGB(1,2,3).Print(false), "38;5;16");
}

TEST(ColorTest, FallbackTo16) {
  Terminal::SetColorSupport(Terminal::Color::Palette16);
  EXPECT_EQ(Color::RGB(1,2,3).Print(false), "30");
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
