// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for text, vbox, separator, separatorDouble, separatorEmpty, separatorHeavy, separatorLight, separatorStyled, DOUBLE
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen, Pixel

// NOLINTBEGIN
namespace ftxui {

TEST(SeparatorTest, Default) {
  auto element = vbox({
      text("top"),
      separator(),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "────\r\n"
            "down");
}

TEST(SeparatorTest, Light) {
  auto element = vbox({
      text("top"),
      separatorLight(),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "────\r\n"
            "down");
}

TEST(SeparatorTest, Dashed) {
  auto element = vbox({
      text("top"),
      separatorDashed(),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "╍╍╍╍\r\n"
            "down");
}

TEST(SeparatorTest, Double) {
  auto element = vbox({
      text("top"),
      separatorDouble(),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "════\r\n"
            "down");
}

TEST(SeparatorTest, Heavy) {
  auto element = vbox({
      text("top"),
      separatorHeavy(),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "━━━━\r\n"
            "down");
}

TEST(SeparatorTest, Empty) {
  auto element = vbox({
      text("top"),
      separatorEmpty(),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "    \r\n"
            "down");
}

TEST(SeparatorTest, Styled) {
  auto element = vbox({
      text("top"),
      separatorStyled(DOUBLE),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "════\r\n"
            "down");
}

TEST(SeparatorTest, WithPixel) {
  Pixel pixel;
  pixel.character = "o";
  auto element = vbox({
      text("top"),
      separator(pixel),
      text("down"),
  });
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "top \r\n"
            "oooo\r\n"
            "down");
}

}  // namespace ftxui
// NOLINTEND
