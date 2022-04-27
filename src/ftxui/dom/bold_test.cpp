#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFactoryImpl, TestPartResult
#include <string>                   // for allocator
#include "ftxui/dom/elements.hpp"   // for text, flexbox
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

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
