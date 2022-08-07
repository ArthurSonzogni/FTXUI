#include <gtest/gtest.h>
#include <string>  // for allocator

#include "ftxui/dom/elements.hpp"   // for operator|, text, bold, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen, Pixel

namespace ftxui {

TEST(BoldTest, Basic) {
  auto element = text("text") | bold;
  Screen screen(5, 1);
  Render(screen, element);
  EXPECT_TRUE(screen.PixelAt(0, 0).bold);
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
