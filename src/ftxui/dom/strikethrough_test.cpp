// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for operator|, text, strikethrough, Element
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen, Cell
#include "gtest/gtest.h"  // for Test, AssertionResult, EXPECT_TRUE, EXPECT_FALSE, Message, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(StrikethroughTest, Basic) {
  auto element = text("text") | strikethrough;
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_TRUE(screen.CellAt(0, 0).strikethrough);
  EXPECT_TRUE(screen.CellAt(1, 0).strikethrough);
  EXPECT_TRUE(screen.CellAt(2, 0).strikethrough);
  EXPECT_TRUE(screen.CellAt(3, 0).strikethrough);
}

TEST(StrikethroughTest, NotAppliedWithoutDecorator) {
  auto element = text("text");
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_FALSE(screen.CellAt(0, 0).strikethrough);
}

TEST(StrikethroughTest, MultiLine) {
  auto element = vbox({
      text("line1") | strikethrough,
      text("line2"),
  });
  Screen screen(5, 2);
  Render(screen, element);
  EXPECT_TRUE(screen.CellAt(0, 0).strikethrough);
  EXPECT_FALSE(screen.CellAt(0, 1).strikethrough);
}

}  // namespace ftxui
// NOLINTEND
