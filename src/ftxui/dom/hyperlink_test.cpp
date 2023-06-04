#include <gtest/gtest.h>  // for Test, EXPECT_EQ, Message, TestPartResult, TestInfo (ptr only), TEST
#include <string>  // for allocator

#include "ftxui/dom/elements.hpp"  // for operator|, text, bgcolor, color, Element
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"   // for Color, Color::Red, Color::RedLight
#include "ftxui/screen/screen.hpp"  // for Screen, Pixel

// NOLINTBEGIN
namespace ftxui {

TEST(HyperlinkTest, Basic) {
  auto element = hbox({
      text("text 1") | hyperlink("https://a.com"),
      text("text 2") | hyperlink("https://b.com"),
      text("text 3"),
      text("text 4") | hyperlink("https://c.com"),
  });

  Screen screen(6*4, 1);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(0, 0).hyperlink, "https://a.com");
  EXPECT_EQ(screen.PixelAt(5, 0).hyperlink, "https://a.com");
  EXPECT_EQ(screen.PixelAt(6, 0).hyperlink, "https://b.com");
  EXPECT_EQ(screen.PixelAt(11, 0).hyperlink, "https://b.com");

  std::string output = screen.ToString();
  EXPECT_EQ(
      output,
      "\x1B]8;;https://a.com\x1B\\"
      "text 1"
      "\x1B]8;;https://b.com\x1B\\"
      "text 2"
      "\x1B]8;;\x1B\\"
      "text 3"
      "\x1B]8;;https://c.com\x1B\\"
      "text 4"
      "\x1B]8;;\x1B\\");
}

}  // namespace ftxui
// NOLINTEND

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
