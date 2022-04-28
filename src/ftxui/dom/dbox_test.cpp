#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFactoryImpl, TestPartResult
#include <string>                   // for allocator

#include "ftxui/dom/elements.hpp"  // for filler, operator|, text, border, dbox, hbox, vbox, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

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

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
