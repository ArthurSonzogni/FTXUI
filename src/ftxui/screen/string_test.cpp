// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/string.hpp"
#include <gtest/gtest.h>
#include <string>  // for allocator, string
#include "ftxui/screen/string_internal.hpp"

namespace ftxui {

TEST(StringTest, StringWidth) {
  // Basic:
  EXPECT_EQ(0, string_width(""));
  EXPECT_EQ(1, string_width("a"));
  EXPECT_EQ(2, string_width("ab"));
  EXPECT_EQ(1, string_width("⬤"));

  // Fullwidth glyphs:
  EXPECT_EQ(2, string_width("测"));
  EXPECT_EQ(4, string_width("测试"));
  EXPECT_EQ(2, string_width("⚫"));
  EXPECT_EQ(2, string_width("🪐"));

  // Combining characters:
  EXPECT_EQ(1, string_width("ā"));
  EXPECT_EQ(1, string_width("a⃒"));
  EXPECT_EQ(1, string_width("a̗"));
  // Control characters:
  EXPECT_EQ(0, string_width("\1"));
  EXPECT_EQ(2, string_width("a\1a"));
}

TEST(StringTest, Utf8ToGlyphs) {
  using T = std::vector<std::string>;
  // Basic:
  EXPECT_EQ(Utf8ToGlyphs(""), T({}));
  EXPECT_EQ(Utf8ToGlyphs("a"), T({"a"}));
  EXPECT_EQ(Utf8ToGlyphs("ab"), T({"a", "b"}));
  // Fullwidth glyphs:
  EXPECT_EQ(Utf8ToGlyphs("测"), T({"测", ""}));
  EXPECT_EQ(Utf8ToGlyphs("测试"), T({"测", "", "试", ""}));
  // Combining characters:
  EXPECT_EQ(Utf8ToGlyphs("ā"), T({"ā"}));
  EXPECT_EQ(Utf8ToGlyphs("a⃒"), T({"a⃒"}));
  EXPECT_EQ(Utf8ToGlyphs("a̗"), T({"a̗"}));
  // Control characters:
  EXPECT_EQ(Utf8ToGlyphs("\1"), T({}));
  EXPECT_EQ(Utf8ToGlyphs("a\1a"), T({"a", "a"}));
}

TEST(StringTest, GlyphCount) {
  // Basic:
  EXPECT_EQ(GlyphCount(""), 0);
  EXPECT_EQ(GlyphCount("a"), 1);
  EXPECT_EQ(GlyphCount("ab"), 2);
  // Fullwidth glyphs:
  EXPECT_EQ(GlyphCount("测"), 1);
  EXPECT_EQ(GlyphCount("测试"), 2);
  // Combining characters:
  EXPECT_EQ(GlyphCount("ā"), 1);
  EXPECT_EQ(GlyphCount("a⃒"), 1);
  EXPECT_EQ(GlyphCount("a̗"), 1);
  // Control characters:
  EXPECT_EQ(GlyphCount("\1"), 0);
  EXPECT_EQ(GlyphCount("a\1a"), 2);
}

TEST(StringTest, GlyphIterate) {
  // Basic:
  EXPECT_EQ(GlyphIterate("", -1), 0u);
  EXPECT_EQ(GlyphIterate("", 0), 0u);
  EXPECT_EQ(GlyphIterate("", 1), 0u);
  EXPECT_EQ(GlyphIterate("a", 0), 0u);
  EXPECT_EQ(GlyphIterate("a", 1), 1u);
  EXPECT_EQ(GlyphIterate("ab", 0), 0u);
  EXPECT_EQ(GlyphIterate("ab", 1), 1u);
  EXPECT_EQ(GlyphIterate("ab", 2), 2u);
  EXPECT_EQ(GlyphIterate("abc", 0), 0u);
  EXPECT_EQ(GlyphIterate("abc", 1), 1u);
  EXPECT_EQ(GlyphIterate("abc", 2), 2u);
  EXPECT_EQ(GlyphIterate("abc", 3), 3u);
  // Fullwidth glyphs:
  EXPECT_EQ(GlyphIterate("测", 0), 0u);
  EXPECT_EQ(GlyphIterate("测", 1), 3u);
  EXPECT_EQ(GlyphIterate("测试", 0), 0u);
  EXPECT_EQ(GlyphIterate("测试", 1), 3u);
  EXPECT_EQ(GlyphIterate("测试", 2), 6u);
  EXPECT_EQ(GlyphIterate("测试", 1, 3), 6u);
  EXPECT_EQ(GlyphIterate("测试", 1, 0), 3u);
  // Combining characters:
  EXPECT_EQ(GlyphIterate("ā", 0), 0u);
  EXPECT_EQ(GlyphIterate("ā", 1), 3u);
  EXPECT_EQ(GlyphIterate("a⃒a̗ā", 0), 0u);
  EXPECT_EQ(GlyphIterate("a⃒a̗ā", 1), 4u);
  EXPECT_EQ(GlyphIterate("a⃒a̗ā", 2), 7u);
  EXPECT_EQ(GlyphIterate("a⃒a̗ā", 3), 10u);
  // Control characters:
  EXPECT_EQ(GlyphIterate("\1", 0), 0u);
  EXPECT_EQ(GlyphIterate("\1", 1), 1u);
  EXPECT_EQ(GlyphIterate("a\1a", 0), 0u);
  EXPECT_EQ(GlyphIterate("a\1a", 1), 2u);
  EXPECT_EQ(GlyphIterate("a\1a", 2), 3u);
}

TEST(StringTest, CellToGlyphIndex) {
  // Basic:
  auto basic = CellToGlyphIndex("abc");
  ASSERT_EQ(basic.size(), 3u);
  EXPECT_EQ(basic[0], 0);
  EXPECT_EQ(basic[1], 1);
  EXPECT_EQ(basic[2], 2);

  // Fullwidth glyphs:
  auto fullwidth = CellToGlyphIndex("测试");
  ASSERT_EQ(fullwidth.size(), 4u);
  EXPECT_EQ(fullwidth[0], 0);
  EXPECT_EQ(fullwidth[1], 0);
  EXPECT_EQ(fullwidth[2], 1);
  EXPECT_EQ(fullwidth[3], 1);

  // Combining characters:
  auto combining = CellToGlyphIndex("a⃒a̗ā");
  ASSERT_EQ(combining.size(), 3u);
  EXPECT_EQ(combining[0], 0);
  EXPECT_EQ(combining[1], 1);
  EXPECT_EQ(combining[2], 2);
}

TEST(StringTest, Utf8ToWordBreakProperty) {
  using T = std::vector<WordBreakProperty>;
  using P = WordBreakProperty;
  EXPECT_EQ(Utf8ToWordBreakProperty("a"), T({P::ALetter}));
  EXPECT_EQ(Utf8ToWordBreakProperty("0"), T({P::Numeric}));
  EXPECT_EQ(Utf8ToWordBreakProperty("א"), T({P::Hebrew_Letter}));
  EXPECT_EQ(Utf8ToWordBreakProperty("ㇰ"), T({P::Katakana}));
  EXPECT_EQ(Utf8ToWordBreakProperty(" "), T({P::WSegSpace}));
  EXPECT_EQ(Utf8ToWordBreakProperty("\""), T({P::Double_Quote}));
  EXPECT_EQ(Utf8ToWordBreakProperty("'"), T({P::Single_Quote}));
  EXPECT_EQ(Utf8ToWordBreakProperty(":"), T({P::MidLetter}));
  EXPECT_EQ(Utf8ToWordBreakProperty("."), T({P::MidNumLet}));
  EXPECT_EQ(Utf8ToWordBreakProperty("\r"), T({}));  // FIXME
  EXPECT_EQ(Utf8ToWordBreakProperty("\n"), T({P::LF}));
}

TEST(StringTest, to_string) {
  EXPECT_EQ(to_string(L"hello"), "hello");
  EXPECT_EQ(to_string(L"€"), "€");
  EXPECT_EQ(to_string(L"ÿ"), "ÿ");
  EXPECT_EQ(to_string(L"߿"), "߿");
  EXPECT_EQ(to_string(L"ɰɱ"), "ɰɱ");
  EXPECT_EQ(to_string(L"«»"), "«»");
  EXPECT_EQ(to_string(L"嵰嵲嵫"), "嵰嵲嵫");
  EXPECT_EQ(to_string(L"🎅🎄"), "🎅🎄");
}

TEST(StringTest, to_wstring) {
  EXPECT_EQ(to_wstring("hello"), L"hello");
  EXPECT_EQ(to_wstring("€"), L"€");
  EXPECT_EQ(to_wstring("ÿ"), L"ÿ");
  EXPECT_EQ(to_wstring("߿"), L"߿");
  EXPECT_EQ(to_wstring("ɰɱ"), L"ɰɱ");
  EXPECT_EQ(to_wstring("«»"), L"«»");
  EXPECT_EQ(to_wstring("嵰嵲嵫"), L"嵰嵲嵫");
  EXPECT_EQ(to_wstring("🎅🎄"), L"🎅🎄");

  // Regression test: check that std::string is correctly matched
  std::string std_str = "hello";
  EXPECT_EQ(to_wstring(std_str), L"hello");
}

}  // namespace ftxui
