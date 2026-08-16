// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>

#include "ftxui/dom/elements.hpp"   // for gauge, gaugeUp
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "ftxui/screen/terminal.hpp"

// NOLINTBEGIN
namespace ftxui {

TEST(GaugeTest, ZeroHorizontal) {
  auto root = gauge(0);
  Screen screen(11, 1);
  Render(screen, root);

  EXPECT_EQ("           ", screen.ToString());
}

TEST(GaugeTest, HalfHorizontal) {
  auto root = gauge(0.5);
  Screen screen(11, 1);
  Render(screen, root);

  if (Terminal::GetQuirks().BlockCharacters()) {
    EXPECT_EQ("█████▍     ", screen.ToString());
  } else {
    EXPECT_EQ("█████▌     ", screen.ToString());
  }
}

TEST(GaugeTest, OneHorizontal) {
  auto root = gauge(1.0);
  Screen screen(11, 1);
  Render(screen, root);

  EXPECT_EQ("███████████", screen.ToString());
}

TEST(GaugeTest, ZeroVertical) {
  auto root = gaugeUp(0);
  Screen screen(1, 11);
  Render(screen, root);

  EXPECT_EQ(
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " ",
      screen.ToString());
}

TEST(GaugeTest, HalfVertical) {
  auto root = gaugeUp(0.5);
  Screen screen(1, 11);
  Render(screen, root);

  EXPECT_EQ(
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      "▄\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█",
      screen.ToString());
}

TEST(GaugeTest, OneVertical) {
  auto root = gaugeUp(1.0);
  Screen screen(1, 11);
  Render(screen, root);

  EXPECT_EQ(
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█\r\n"
      "█",
      screen.ToString());
}

TEST(GaugeTest, CustomTwoCharHorizontal) {
  auto root = gaugeCharset(0.5, {" ", "#"});
  Screen screen(10, 1);
  Render(screen, root);

  EXPECT_EQ("#####     ", screen.ToString());
}

TEST(GaugeTest, CustomVectorHorizontal) {
  auto root = gaugeCharset(0.5, {" ", "░", "▒", "▓", "█"});
  Screen screen(10, 1);
  Render(screen, root);

  EXPECT_EQ("█████     ", screen.ToString());
}

TEST(GaugeTest, CustomCharsetInvertsLikeDefault) {
  // gaugeCharset(..., Direction::Left) renders the same glyphs as
  // Direction::Right, then flips each cell's `inverted` flag. A default
  // gauge does this too; both must match.
  auto default_gauge = gaugeLeft(0.5);
  auto custom_gauge = gaugeCharset(0.5, {" ", "█"}, Direction::Left);
  Screen default_screen(10, 1);
  Screen custom_screen(10, 1);
  Render(default_screen, default_gauge);
  Render(custom_screen, custom_gauge);

  for (int x = 0; x < 10; ++x) {
    EXPECT_EQ(default_screen.PixelAt(x, 0).inverted,
              custom_screen.PixelAt(x, 0).inverted)
        << "at x=" << x;
  }
}

}  // namespace ftxui
// NOLINTEND
