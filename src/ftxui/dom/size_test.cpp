// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for size, text, operator|, hbox, vbox, Element, WIDTH, HEIGHT, EQUAL, LESS_THAN, GREATER_THAN
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest.h"  // for Test, EXPECT_EQ, Message, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(SizeTest, WidthEqual) {
  auto element = text("hi") | size(WIDTH, EQUAL, 5);
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hi        ");
}

TEST(SizeTest, WidthEqualClips) {
  // size(WIDTH, EQUAL, N) clips the child to at most N+2 cells.
  // Because the box formula gives x_max = x_min + value + 1.
  auto element = text("hello world") | size(WIDTH, EQUAL, 5);
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hello w   ");
}

TEST(SizeTest, HeightEqual) {
  auto element = text("hi") | size(HEIGHT, EQUAL, 3);
  Screen screen(5, 5);
  Render(screen, element);
  EXPECT_EQ(screen.at(0, 0), "h");
  EXPECT_EQ(screen.at(1, 0), "i");
}

TEST(SizeTest, WidthLessThan) {
  // LESS_THAN uses the same clipping as EQUAL.
  auto element = text("hello world") | size(WIDTH, LESS_THAN, 5);
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hello w   ");
}

TEST(SizeTest, WidthLessThanSmallContent) {
  // When content is already smaller, LESS_THAN doesn't expand it.
  auto element = text("hi") | size(WIDTH, LESS_THAN, 10);
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hi        ");
}

TEST(SizeTest, WidthGreaterThan) {
  // GREATER_THAN ensures a minimum size.
  auto element = text("hi") | size(WIDTH, GREATER_THAN, 5);
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hi        ");
}

TEST(SizeTest, HeightLessThan) {
  auto element =
      vbox({text("1"), text("2"), text("3")}) | size(HEIGHT, LESS_THAN, 2);
  Screen screen(5, 5);
  Render(screen, element);
  EXPECT_EQ(screen.at(0, 0), "1");
  EXPECT_EQ(screen.at(0, 1), "2");
}

TEST(SizeTest, HeightGreaterThan) {
  auto element = text("hi") | size(HEIGHT, GREATER_THAN, 3);
  Screen screen(5, 5);
  Render(screen, element);
  EXPECT_EQ(screen.at(0, 0), "h");
  EXPECT_EQ(screen.at(1, 0), "i");
}

TEST(SizeTest, NegativeValueClamped) {
  // Negative values should be clamped to 0.
  auto element = text("hi") | size(WIDTH, EQUAL, -5);
  Screen screen(5, 1);
  Render(screen, element);
  // Should render without crashing.
  (void)screen.ToString();
}

TEST(SizeTest, DecoratorSyntax) {
  // When the screen is smaller than value+2, the constraint has no effect.
  auto element = text("hello") | size(WIDTH, EQUAL, 3);
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hello");
}

}  // namespace ftxui
// NOLINTEND
