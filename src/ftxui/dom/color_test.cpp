#include <gtest/gtest.h>  // for Test, EXPECT_EQ, Message, TestPartResult, TestInfo (ptr only), TEST
#include <ftxui/dom/linear_gradient.hpp>  // for LinearGradient::Stop, LinearGradient
#include <string>                         // for allocator

#include "ftxui/dom/elements.hpp"  // for operator|, text, bgcolor, color, Element
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"   // for Color, Color::Red, Color::RedLight
#include "ftxui/screen/screen.hpp"  // for Screen, Pixel

namespace ftxui {

TEST(ColorTest, Foreground) {
  auto element = text("text") | color(Color::Red);
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_EQ(screen.PixelAt(0, 0).foreground_color, Color::Red);
  EXPECT_EQ(screen.PixelAt(0, 0).background_color, Color());
}

TEST(ColorTest, Background) {
  auto element = text("text") | bgcolor(Color::Red);
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_EQ(screen.PixelAt(0, 0).foreground_color, Color());
  EXPECT_EQ(screen.PixelAt(0, 0).background_color, Color::Red);
}

TEST(ColorTest, GradientForeground) {
  auto element =
      text("text") |
      color(LinearGradient{0.f, {{Color::RedLight, 0.f}, {Color::Red, 1.f}}});
  Screen screen(5, 1);
  Render(screen, element);

  Color gradient_begin = Color::Interpolate(0, Color::RedLight, Color::Red);
  Color gradient_end = Color::Interpolate(1, Color::RedLight, Color::Red);

  EXPECT_EQ(screen.PixelAt(0, 0).foreground_color, gradient_begin);
  EXPECT_EQ(screen.PixelAt(0, 0).background_color, Color());

  EXPECT_EQ(screen.PixelAt(4, 0).foreground_color, gradient_end);
  EXPECT_EQ(screen.PixelAt(4, 0).background_color, Color());
}

TEST(ColorTest, GradientBackground) {
  auto element =
      text("text") |
      bgcolor(LinearGradient{0.f, {{Color::RedLight, 0.f}, {Color::Red, 1.f}}});
  Screen screen(5, 1);
  Render(screen, element);

  Color gradient_begin = Color::Interpolate(0, Color::RedLight, Color::Red);
  Color gradient_end = Color::Interpolate(1, Color::RedLight, Color::Red);

  EXPECT_EQ(screen.PixelAt(0, 0).foreground_color, Color());
  EXPECT_EQ(screen.PixelAt(0, 0).background_color, gradient_begin);

  EXPECT_EQ(screen.PixelAt(4, 0).foreground_color, Color());
  EXPECT_EQ(screen.PixelAt(4, 0).background_color, gradient_end);
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
