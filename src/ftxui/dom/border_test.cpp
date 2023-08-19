// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for text, operator|, Element, borderStyled, borderWith, window, border, borderDouble, borderEmpty, borderHeavy, borderLight, borderRounded, DOUBLE
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen, Pixel

// NOLINTBEGIN
namespace ftxui {

TEST(BorderTest, Default) {
  auto element = text("text") | border;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "╭───╮\r\n"
            "│tex│\r\n"
            "╰───╯");
}

TEST(BorderTest, Light) {
  auto element = text("text") | borderLight;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "┌───┐\r\n"
            "│tex│\r\n"
            "└───┘");
}

TEST(BorderTest, Double) {
  auto element = text("text") | borderDouble;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "╔═══╗\r\n"
            "║tex║\r\n"
            "╚═══╝");
}

TEST(BorderTest, Rounded) {
  auto element = text("text") | borderRounded;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "╭───╮\r\n"
            "│tex│\r\n"
            "╰───╯");
}

TEST(BorderTest, Heavy) {
  auto element = text("text") | borderHeavy;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "┏━━━┓\r\n"
            "┃tex┃\r\n"
            "┗━━━┛");
}

TEST(BorderTest, Empty) {
  auto element = text("text") | borderEmpty;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "     \r\n"
            " tex \r\n"
            "     ");
}

TEST(BorderTest, Styled) {
  auto element = text("text") | borderStyled(DOUBLE);
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "╔═══╗\r\n"
            "║tex║\r\n"
            "╚═══╝");
}

TEST(BorderTest, WithPixel) {
  Pixel pixel;
  pixel.character = "o";
  auto element = text("text") | borderWith(pixel);
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "ooooo\r\n"
            "otexo\r\n"
            "ooooo");
}

TEST(BorderTest, Window) {
  auto element = window(text("title"), text("content"));
  Screen screen(10, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "╭title───╮\r\n"
            "│content │\r\n"
            "╰────────╯");
}

}  // namespace ftxui
// NOLINTEND
