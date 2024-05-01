// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_STRING_INTERNAL_HPP
#define FTXUI_SCREEN_STRING_INTERNAL_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ftxui {

bool EatCodePoint(const std::string& input,
                  size_t start,
                  size_t* end,
                  uint32_t* ucs);
bool EatCodePoint(const std::wstring& input,
                  size_t start,
                  size_t* end,
                  uint32_t* ucs);

bool IsCombining(uint32_t ucs);
bool IsFullWidth(uint32_t ucs);
bool IsControl(uint32_t ucs);

size_t GlyphPrevious(const std::string& input, size_t start);
size_t GlyphNext(const std::string& input, size_t start);

// Return the index in the |input| string of the glyph at |glyph_offset|,
// starting at |start|
size_t GlyphIterate(const std::string& input,
                    int glyph_offset,
                    size_t start = 0);

// Returns the number of glyphs in |input|.
int GlyphCount(const std::string& input);

// Properties from:
// https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/WordBreakProperty.txt
enum class WordBreakProperty : int8_t {
  ALetter,
  CR,
  Double_Quote,
  Extend,
  ExtendNumLet,
  Format,
  Hebrew_Letter,
  Katakana,
  LF,
  MidLetter,
  MidNum,
  MidNumLet,
  Newline,
  Numeric,
  Regional_Indicator,
  Single_Quote,
  WSegSpace,
  ZWJ,
};
WordBreakProperty CodepointToWordBreakProperty(uint32_t codepoint);
std::vector<WordBreakProperty> Utf8ToWordBreakProperty(
    const std::string& input);

bool IsWordBreakingCharacter(const std::string& input, size_t glyph_index);
}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_STRING_INTERNAL_HPP */
