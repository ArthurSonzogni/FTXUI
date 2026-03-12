// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>  // for allocator

#include "ftxui/dom/elements.hpp"  // for filler, operator|, text, border, dbox, hbox, vbox, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {

TEST(DBoxTest, Basic) {
  auto root = dbox({
      hbox({
          text("test") | border,
          filler(),
      }),
      vbox({
          text("test") | border,
          filler(),
      }),
  });

  Screen screen(8, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭────┬─╮\r\n"
            "│test│ │\r\n"
            "╰────┴─╯\r\n"
            "╰────╯  ");
}

// Ensures that the focused element is the one with the longest text, even if it
// is not the first one in the list.
TEST(DBoxTest, FocusPropagation) {
  auto bottom = text("bottom--------------") | focus;
  auto middle = text("middle--------") | focus;
  auto top = text("top----") | focus;
  auto root = dbox({bottom, middle, top});
  root->ComputeRequirement();
  EXPECT_EQ(root->requirement().focused.box.x_max, 6);
}

}  // namespace ftxui
// NOLINTEND
