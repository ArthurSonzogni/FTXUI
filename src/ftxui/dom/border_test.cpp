// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for text, operator|, Element, borderStyled, borderWith, window, border, borderDouble, borderEmpty, borderHeavy, borderLight, borderRounded, DOUBLE
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"   // for Color
#include "ftxui/screen/screen.hpp"  // for Screen, Cell

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
  Cell pixel;
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

// https://github.com/ArthurSonzogni/FTXUI/issues/1016
TEST(BorderTest, WindowTitleCentered) {
  auto element = window(text("title") | hcenter, text("content"));
  Screen screen(11, 3);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(),
            "╭──title──╮\r\n"
            "│content  │\r\n"
            "╰─────────╯");
}

// https://github.com/ArthurSonzogni/FTXUI/issues/940
TEST(BorderTest, WindowTitleColorOnlyAppliesToTitle) {
  auto element =
      window(text("title") | hcenter | color(Color::Red), text("content"));
  Screen screen(11, 3);
  Render(screen, element);
  for (int x = 0; x < 11; ++x) {
    const bool is_title = x >= 3 && x < 8;
    EXPECT_EQ(screen.CellAt(x, 0).foreground_color,
              is_title ? Color(Color::Red) : Color(Color::Default));
  }
}

}  // namespace ftxui
// NOLINTEND
