// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/color.hpp"
#include <gtest/gtest.h>
#include "ftxui/screen/terminal.hpp"

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
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "38;2;1;2;3");
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(true), "48;2;1;2;3");
}

TEST(ColorTest, FallbackTo256) {
  Terminal::SetColorSupport(Terminal::Color::Palette256);
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "38;5;16");
}

TEST(ColorTest, FallbackTo16) {
  Terminal::SetColorSupport(Terminal::Color::Palette16);
  EXPECT_EQ(Color::RGB(1, 2, 3).Print(false), "30");
}

TEST(ColorTest, Literals) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  using namespace ftxui::literals;
  EXPECT_EQ(Color(0xABCDEF_rgb).Print(false), "38;2;171;205;239");
}

TEST(ColorTest, Interpolate) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);

  EXPECT_EQ(Color::Interpolate(0.3f, Color(), Color()).Print(false), "39");
  EXPECT_EQ(Color::Interpolate(0.3f, Color::Red, Color()).Print(false), "31");
  EXPECT_EQ(Color::Interpolate(0.7f, Color::Red, Color()).Print(false), "39");
  EXPECT_EQ(Color::Interpolate(0.3f, Color(), Color::Red).Print(false), "39");
  EXPECT_EQ(Color::Interpolate(0.7f, Color(), Color::Red).Print(false), "31");

  EXPECT_EQ(Color::Interpolate(0.3f,                       //
                               Color::RGB(1, 2, 3),        //
                               Color::RGB(244, 244, 123))  //
                .Print(false),
            "38;2;141;141;71");
  EXPECT_EQ(Color::Interpolate(0.7f,                       //
                               Color::RGB(1, 2, 3),        //
                               Color::RGB(244, 244, 123))  //
                .Print(false),
            "38;2;207;207;104");
  EXPECT_EQ(Color::Interpolate(0.7f,                       //
                               Color(Color::Red),          //
                               Color::RGB(244, 244, 123))  //
                .Print(false),
            "38;2;216;207;104");
  EXPECT_EQ(Color::Interpolate(0.7f,                       //
                               Color::RGB(244, 244, 123),  //
                               Color(Color::Plum1))        //
                .Print(false),
            "38;2;251;198;225");
}

TEST(ColorTest, HSV) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(Color::HSV(0, 255, 255).Print(false), "38;2;255;0;0");
}

TEST(ColorTest, ComputeColorSupport) {
#if !defined(_WIN32)
  // Only check Palette256/Palette16 expectations if WT_SESSION is not set.
  const char* wt_session = std::getenv("WT_SESSION");  // NOLINT
  if (wt_session == nullptr || wt_session[0] == '\0') {
    EXPECT_EQ(Terminal::ComputeColorSupport("xterm", "256", "", "", "", {}),
              Terminal::Color::Palette256);
    EXPECT_EQ(Terminal::ComputeColorSupport("xterm-256color", "", "", "", "", {}),
              Terminal::Color::Palette256);
  }
#endif
  EXPECT_EQ(Terminal::ComputeColorSupport("xterm", "truecolor", "", "", "", {}),
            Terminal::Color::TrueColor);
  EXPECT_EQ(Terminal::ComputeColorSupport("kitty", "", "", "", "", {}),
            Terminal::Color::TrueColor);
}

#if !defined(_WIN32)
TEST(ColorTest, ComputeColorSupportWTSession) {
  // Save the current WT_SESSION state.
  const char* original = std::getenv("WT_SESSION");  // NOLINT
  std::string original_str = original ? original : "";

  setenv("WT_SESSION", "test_session_id", 1);
  EXPECT_EQ(Terminal::ComputeColorSupport("", "", "", "", "", {}),
            Terminal::Color::TrueColor);

  if (original) {
    setenv("WT_SESSION", original_str.c_str(), 1);
  } else {
    unsetenv("WT_SESSION");
  }
}
#endif

}  // namespace ftxui
