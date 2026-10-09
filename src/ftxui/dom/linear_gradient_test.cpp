// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>  // for Test, EXPECT_EQ, Message, TestPartResult, TestInfo (ptr only), TEST
#include <ftxui/dom/linear_gradient.hpp>  // for LinearGradient::Stop, LinearGradient

#include "ftxui/dom/elements.hpp"  // for operator|, text, bgcolor, color, Element
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"   // for Color, Color::RedLight, Color::Red
#include "ftxui/screen/screen.hpp"  // for Screen, Cell

// NOLINTBEGIN
namespace ftxui {

TEST(ColorTest, API_default) {
  LinearGradient gradient;
  EXPECT_EQ(gradient.angle, 0);
  EXPECT_EQ(gradient.stops.size(), 0u);
}

TEST(ColorTest, API_builder) {
  auto gradient = LinearGradient()  //
                      .Angle(45)
                      .Stop(Color::Red)
                      .Stop(Color::RedLight, 0.5)
                      .Stop(Color::RedLight);
  EXPECT_EQ(gradient.angle, 45);
  EXPECT_EQ(gradient.stops.size(), 3u);
  EXPECT_EQ(gradient.stops[0].color, Color::Red);
  EXPECT_EQ(gradient.stops[0].position, std::nullopt);
  EXPECT_EQ(gradient.stops[1].color, Color::RedLight);
  EXPECT_EQ(gradient.stops[1].position, 0.5);
  EXPECT_EQ(gradient.stops[2].color, Color::RedLight);
  EXPECT_EQ(gradient.stops[2].position, std::nullopt);
}

TEST(ColorTest, API_constructor_bicolor) {
  auto gradient = LinearGradient(Color::Red, Color::RedLight);
  EXPECT_EQ(gradient.angle, 0);
  EXPECT_EQ(gradient.stops.size(), 2u);
  EXPECT_EQ(gradient.stops[0].color, Color::Red);
  EXPECT_EQ(gradient.stops[0].position, std::nullopt);
  EXPECT_EQ(gradient.stops[1].color, Color::RedLight);
  EXPECT_EQ(gradient.stops[1].position, std::nullopt);
}

TEST(ColorTest, API_constructor_bicolor_angle) {
  auto gradient = LinearGradient(45, Color::Red, Color::RedLight);
  EXPECT_EQ(gradient.angle, 45);
  EXPECT_EQ(gradient.stops.size(), 2u);
  EXPECT_EQ(gradient.stops[0].color, Color::Red);
  EXPECT_EQ(gradient.stops[0].position, std::nullopt);
  EXPECT_EQ(gradient.stops[1].color, Color::RedLight);
  EXPECT_EQ(gradient.stops[1].position, std::nullopt);
}

TEST(ColorTest, GradientForeground) {
  auto element =
      text("text") | color(LinearGradient(Color::RedLight, Color::Red));
  Screen screen(5, 1);
  Render(screen, element);

  Color gradient_begin = Color::Interpolate(0, Color::RedLight, Color::Red);
  Color gradient_end = Color::Interpolate(1, Color::RedLight, Color::Red);

  EXPECT_EQ(screen.CellAt(0, 0).foreground_color, gradient_begin);
  EXPECT_EQ(screen.CellAt(0, 0).background_color, Color());

  EXPECT_EQ(screen.CellAt(4, 0).foreground_color, gradient_end);
  EXPECT_EQ(screen.CellAt(4, 0).background_color, Color());
}

TEST(ColorTest, GradientBackground) {
  auto element =
      text("text") | bgcolor(LinearGradient(Color::RedLight, Color::Red));
  Screen screen(5, 1);
  Render(screen, element);

  Color gradient_begin = Color::Interpolate(0, Color::RedLight, Color::Red);
  Color gradient_end = Color::Interpolate(1, Color::RedLight, Color::Red);

  EXPECT_EQ(screen.CellAt(0, 0).foreground_color, Color());
  EXPECT_EQ(screen.CellAt(0, 0).background_color, gradient_begin);

  EXPECT_EQ(screen.CellAt(4, 0).foreground_color, Color());
  EXPECT_EQ(screen.CellAt(4, 0).background_color, gradient_end);
}

// https://github.com/ArthurSonzogni/FTXUI/issues/1379
TEST(ColorTest, GradientImplicitPositions) {
  auto implicit = LinearGradient()
                      .Stop(Color::Red)
                      .Stop(Color::Green)
                      .Stop(Color::Blue)
                      .Stop(Color::White);
  auto explicit_ = LinearGradient()
                       .Stop(Color::Red, 0.F)
                       .Stop(Color::Green, 1.F / 3.F)
                       .Stop(Color::Blue, 2.F / 3.F)
                       .Stop(Color::White, 1.F);

  Screen screen_implicit(7, 1);
  Screen screen_explicit(7, 1);
  Render(screen_implicit, text("       ") | bgcolor(implicit));
  Render(screen_explicit, text("       ") | bgcolor(explicit_));

  for (int x = 0; x < 7; ++x) {
    EXPECT_EQ(screen_implicit.CellAt(x, 0).background_color,
              screen_explicit.CellAt(x, 0).background_color);
  }
}

}  // namespace ftxui
// NOLINTEND
