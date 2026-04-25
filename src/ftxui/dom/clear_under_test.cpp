// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for clear_under, dbox, text, color, operator|, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/color.hpp"   // for Color, Color::Red
#include "ftxui/screen/screen.hpp"  // for Screen, Cell
#include "gtest/gtest.h"  // for Test, EXPECT_EQ, EXPECT_NE, Message, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(ClearUnderTest, Basic) {
  // Background text set with color, overlaid with clear_under + new text.
  auto bottom = text("AAAA") | color(Color::Red);
  auto top = text("BB") | clear_under;
  auto root = dbox({bottom, top});

  Screen screen(4, 1);
  Render(screen, root);

  // The top "BB" with clear_under should have overwritten the cells.
  EXPECT_EQ(screen.at(0, 0), "B");
  EXPECT_EQ(screen.at(1, 0), "B");
}

TEST(ClearUnderTest, ClearsAttributes) {
  // Verify that clear_under clears cell attributes (e.g., bold) from below.
  auto bottom = text("AAAA") | bold;
  auto top = text("BB") | clear_under;
  auto root = dbox({bottom, top});

  Screen screen(4, 1);
  Render(screen, root);

  // Cells under "BB" should be cleared (bold reset by clear_under).
  EXPECT_FALSE(screen.CellAt(0, 0).bold);
  EXPECT_FALSE(screen.CellAt(1, 0).bold);
}

TEST(ClearUnderTest, OnlyAffectsOwnArea) {
  // Use hbox so clear_under gets only the left half of the cells.
  // Bottom: bold text across full 4 cells.
  // Top: hbox with clear_under on left 2 cells, plain text on right 2.
  auto bottom = text("AAAA") | bold;
  auto top = hbox({
      text("BB") | clear_under,
      text("CC"),
  });
  auto root = dbox({bottom, top});

  Screen screen(4, 1);
  Render(screen, root);

  // Cells 0,1 are under clear_under: bold cleared.
  EXPECT_FALSE(screen.CellAt(0, 0).bold);
  EXPECT_FALSE(screen.CellAt(1, 0).bold);
  // Cells 2,3 are NOT under clear_under: bold from bottom retained.
  EXPECT_TRUE(screen.CellAt(2, 0).bold);
  EXPECT_TRUE(screen.CellAt(3, 0).bold);
}

TEST(ClearUnderTest, MultiLine) {
  // clear_under clears all cells within its box_ (both rows in a 4x2 dbox).
  auto bottom = vbox({
      text("AAAA") | bold,
      text("BBBB") | bold,
  });
  auto top = text("XX") | clear_under;
  auto root = dbox({bottom, top});

  Screen screen(4, 2);
  Render(screen, root);

  // clear_under operates on the full dbox area (both rows), clearing bold.
  EXPECT_FALSE(screen.CellAt(0, 0).bold);
  EXPECT_FALSE(screen.CellAt(0, 1).bold);
}

}  // namespace ftxui
// NOLINTEND
