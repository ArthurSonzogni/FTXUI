// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"   // for operator|, text, inverted, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen, Cell
#include "gtest/gtest.h"  // for Test, AssertionResult, EXPECT_TRUE, EXPECT_FALSE, Message, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(InvertedTest, Basic) {
  auto element = text("text") | inverted;
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_TRUE(screen.CellAt(0, 0).inverted);
  EXPECT_TRUE(screen.CellAt(1, 0).inverted);
  EXPECT_TRUE(screen.CellAt(2, 0).inverted);
  EXPECT_TRUE(screen.CellAt(3, 0).inverted);
}

TEST(InvertedTest, NotAppliedWithoutDecorator) {
  auto element = text("text");
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_FALSE(screen.CellAt(0, 0).inverted);
}

TEST(InvertedTest, DoubleInverted) {
  // Applying inverted twice should cancel out (XOR logic).
  auto element = text("text") | inverted | inverted;
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_FALSE(screen.CellAt(0, 0).inverted);
}

TEST(InvertedTest, MultiLine) {
  auto element = vbox({
      text("line1") | inverted,
      text("line2"),
  });
  Screen screen(5, 2);
  Render(screen, element);
  EXPECT_TRUE(screen.CellAt(0, 0).inverted);
  EXPECT_FALSE(screen.CellAt(0, 1).inverted);
}

}  // namespace ftxui
// NOLINTEND
