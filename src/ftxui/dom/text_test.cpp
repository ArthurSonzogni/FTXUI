// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>       // for allocator, string
#include <string_view>  // for string_view

#include "ftxui/dom/elements.hpp"   // for text, operator|, border, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {

TEST(TextTest, ScreenHeightSmaller) {
  auto element = text("test");
  Screen screen(2, 0);
  Render(screen, element);

  EXPECT_EQ("", screen.ToString());
}

TEST(TextTest, ScreenSmaller) {
  auto element = text("test");
  Screen screen(2, 1);
  Render(screen, element);

  EXPECT_EQ("te", screen.ToString());
}

TEST(TextTest, ScreenFit) {
  auto element = text("test");
  Screen screen(4, 1);
  Render(screen, element);

  EXPECT_EQ("test", screen.ToString());
}

TEST(TextTest, ScreenBigger) {
  auto element = text("test");
  Screen screen(6, 1);
  Render(screen, element);

  EXPECT_EQ("test  ", screen.ToString());
}

TEST(TextTest, ScreenBigger2) {
  auto element = text("test");
  Screen screen(6, 2);
  Render(screen, element);

  EXPECT_EQ("test  \r\n      ", screen.ToString());
}

// See https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-504871456
TEST(TextTest, CJK) {
  auto element = text("测试") | border;
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
  auto element = text("测试") | border;
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
  auto element = text("测试") | border;
  Screen screen(4, 3);
  Render(screen, element);
  EXPECT_EQ(
      "╭──╮\r\n"
      "│测│\r\n"
      "╰──╯",
      screen.ToString());
}

TEST(TextTest, CombiningCharacters) {
  const std::string t =
      // Combining above:
      "āàáâãāa̅ăȧäảåa̋ǎa̍a̎ȁa̐ȃa̒a̔a̕a̚a̛a̽a̾a̿àáa͂a͆a͊a͋a͌a͐"
      "a͑a͒a͗a͘a͛a͝a͞a͠a͡aͣaͤaͥaͦaͧaͨaͩaͪaͫaͬaͭaͮaͯa᷀a᷁a᷃a᷄a᷅a᷆a᷇a᷈a᷉a᷾a⃐a⃑a⃔"
      "a⃕a⃖a⃗a⃛a⃜a⃡a⃩a⃰a︠a︡a︢a︣"
      // Combining middle:
      "a̴a̵a̶a̷a̸a⃒a⃓a⃘a⃙a⃚a⃝a⃞a⃟a⃥a⃦"
      // Combining below:
      "a̗a̘a̙a̜a̝a̞a̟a̠a̡a̢ạḁa̦a̧ąa̩a̪a̫a̬a̭a̮a̯a̰a̱a̲a̳a̹a̺a̻a̼aͅa͇a͈a͉a͍"
      "a͎a͓a͔a͕a͖a͙a͚a͜a͟a͢a᷂a᷊a᷿a⃨";
  auto element = text(t);
  Screen screen(146, 1);
  Render(screen, element);
  EXPECT_EQ(t, screen.ToString());
}

TEST(TextTest, CombiningCharactersWithSpace) {
  const std::string t =
      // Combining above:
      "ā à á â ã ā a̅ ă ȧ ä ả å a̋ ǎ a̍ a̎ ȁ a̐ ȃ a̒ a̔ a̕ a̚ a̛ a̽ a̾ a̿ à á a͂ a͆ a͊ a͋ a͌ a͐ "
      "a͑ a͒ a͗ a͘ a͛ a͝ a͞ a͠ a͡ aͣ aͤ aͥ aͦ aͧ aͨ aͩ aͪ aͫ aͬ aͭ aͮ aͯ a᷀ a᷁ a᷃ a᷄ a᷅ a᷆ a᷇ a᷈ a᷉ a᷾ a⃐ a⃑ a⃔ "
      "a⃕ a⃖ a⃗ a⃛ a⃜ a⃡ a⃩ a⃰ a︠ a︡ a︢ a︣"
      // Combining middle:
      "a̴ a̵ a̶ a̷ a̸ a⃒ a⃓ a⃘ a⃙ a⃚ a⃝ a⃞ a⃟ a⃥ a⃦"
      // Combining below:
      "a̗ a̘ a̙ a̜ a̝ a̞ a̟ a̠ a̡ a̢ ạ ḁ a̦ a̧ ą a̩ a̪ a̫ a̬ a̭ a̮ a̯ a̰ a̱ a̲ a̳ a̹ a̺ a̻ a̼ aͅ a͇ a͈ a͉ a͍ "
      "a͎ a͓ a͔ a͕ a͖ a͙ a͚ a͜ a͟ a͢ a᷂ a᷊ a᷿ a⃨ ";
  auto element = text(t);
  Screen screen(290, 1);
  Render(screen, element);
  EXPECT_EQ(t, screen.ToString());
}

TEST(TextTest, WithStringViews) {
  const std::string_view t = "Hello, world!";
  auto element = text(t);
  Screen screen(13, 1);
  Render(screen, element);
  EXPECT_EQ(t, screen.ToString());
}

TEST(TextTest, Newline) {
  auto element = text("foo\nbar");
  Screen screen(3, 2);
  Render(screen, element);
  EXPECT_EQ("foo\r\nbar", screen.ToString());
}

TEST(TextTest, NewlineScreenSmaller) {
  auto element = text("foo\nbar");
  Screen screen(2, 2);
  Render(screen, element);
  EXPECT_EQ("fo\r\nba", screen.ToString());
}

TEST(TextTest, NewlineScreenTaller) {
  auto element = text("foo\nbar");
  Screen screen(3, 3);
  Render(screen, element);
  EXPECT_EQ("foo\r\nbar\r\n   ", screen.ToString());
}

TEST(TextTest, NewlineMultiple) {
  auto element = text("a\nb\nc");
  Screen screen(1, 3);
  Render(screen, element);
  EXPECT_EQ("a\r\nb\r\nc", screen.ToString());
}

TEST(TextTest, Formatted) {
  auto element = ftext("{} {} {:.2f}",
    "Hello",
    1492,
    2413.13131424234243
  );
  Screen screen(18, 1);
  Render(screen, element);
  EXPECT_EQ("Hello 1492 2413.13", screen.ToString());
}

TEST(TextTest, FormattedWide) {
  auto element = ftext("{}{}{}", "█", "❤️", "猫");
  Screen screen(3, 1);
  Render(screen, element);
  EXPECT_EQ("█❤️猫", screen.ToString());
}

TEST(TextTest, FormattedVertical) {
  auto element = fvtext("{}{}{:.2f}",
    "Hello",
    1492,
    2413.13131424234243
  );
  Screen screen(1, 16);
  Render(screen, element);
  EXPECT_EQ("H\r\ne\r\nl\r\nl\r\no\r\n1\r\n4\r\n9"
            "\r\n2\r\n2\r\n4\r\n1\r\n3\r\n.\r\n1\r\n3",
    screen.ToString());
}

TEST(TextTest, WideFormattedVertical) {
  auto element = fvtext("{}{}{}", "█", "❤️", "猫");
  Screen screen(1, 3);
  Render(screen, element);
  EXPECT_EQ("█\r\n❤️\r\n猫", screen.ToString());
}


}  // namespace ftxui
// NOLINTEND
