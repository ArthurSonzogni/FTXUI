#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFactoryImpl, TestPartResult
#include <string>                   // for allocator, wstring

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/elements.hpp"    // for operator|, border, Element
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui
#include "ftxui/screen/screen.hpp"   // for Screen
#include "ftxui/screen/string.hpp"   // for to_string
#include "gtest/gtest_pred_impl.h"   // for Test, EXPECT_EQ, TEST

using namespace ftxui;

TEST(TextTest, ScreenHeightSmaller) {
  auto element = text(L"test");
  Screen screen(2, 0);
  Render(screen, element);

  EXPECT_EQ("", screen.ToString());
}

TEST(TextTest, ScreenSmaller) {
  auto element = text(L"test");
  Screen screen(2, 1);
  Render(screen, element);

  EXPECT_EQ("te", screen.ToString());
}

TEST(TextTest, ScreenFit) {
  auto element = text(L"test");
  Screen screen(4, 1);
  Render(screen, element);

  EXPECT_EQ("test", screen.ToString());
}

TEST(TextTest, ScreenBigger) {
  auto element = text(L"test");
  Screen screen(6, 1);
  Render(screen, element);

  EXPECT_EQ("test  ", screen.ToString());
}

TEST(TextTest, ScreenBigger2) {
  auto element = text(L"test");
  Screen screen(6, 2);
  Render(screen, element);

  EXPECT_EQ("test  \r\n      ", screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK) {
  auto element = text(L"测试") | border;
  Screen screen(6, 3);
  Render(screen, element);
  EXPECT_EQ(
      "╭────╮\r\n"
      "│测试│\r\n"
      "╰────╯",
      screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK_2) {
  auto element = text(L"测试") | border;
  Screen screen(5, 3);
  Render(screen, element);
  EXPECT_EQ(
      "╭───╮\r\n"
      "│测试\r\n"
      "╰───╯",
      screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK_3) {
  auto element = text(L"测试") | border;
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(
      "╭──╮\r\n"
      "│测│\r\n"
      "╰──╯",
      screen.ToString());
}

TEST(TextTest, CombiningCharacters) {
  const std::wstring t =
      // Combining above:
      L"ā à á â ã ā a̅ ă ȧ ä ả å a̋ ǎ a̍ a̎ ȁ a̐ ȃ a̒ a̔ a̕ a̚ a̛ a̽ a̾ a̿ à á a͂ a͆ a͊ a͋ a͌ a͐ "
      L"a͑ a͒ a͗ a͘ a͛ a͝ a͞ a͠ a͡ aͣ aͤ aͥ aͦ aͧ aͨ aͩ aͪ aͫ aͬ aͭ aͮ aͯ a᷀ a᷁ a᷃ a᷄ a᷅ a᷆ a᷇ a᷈ a᷉ a᷾ a⃐ a⃑ a⃔ "
      L"a⃕ a⃖ a⃗ a⃛ a⃜ a⃡ a⃩ a⃰ a︠ a︡ a︢ a︣"
      // Combining middle:
      L"a̴ a̵ a̶ a̷ a̸ a⃒ a⃓ a⃘ a⃙ a⃚ a⃝ a⃞ a⃟ a⃥ a⃦"
      // Combining below:
      L"a̗ a̘ a̙ a̜ a̝ a̞ a̟ a̠ a̡ a̢ ạ ḁ a̦ a̧ ą a̩ a̪ a̫ a̬ a̭ a̮ a̯ a̰ a̱ a̲ a̳ a̹ a̺ a̻ a̼ aͅ a͇ a͈ a͉ a͍ "
      L"a͎ a͓ a͔ a͕ a͖ a͙ a͚ a͜ a͟ a͢ a᷂ a᷊ a᷿ a⃨";
  auto element = text(t);
  Screen screen(290, 1);
  Render(screen, element);
  EXPECT_EQ(to_string(t), screen.ToString());
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
