// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_STRING_HPP
#define FTXUI_SCREEN_STRING_HPP

#include <string>       // for string, wstring, to_string
#include <string_view>  // for string_view
#include <vector>       // for vector

namespace ftxui {
std::string to_string(std::wstring_view s);
std::wstring to_wstring(std::string_view s);

template <typename T>
std::wstring to_wstring(T s) {
  return to_wstring(std::string_view(std::to_string(s)));
}
template <>
inline std::wstring to_wstring(const char* s) {
  return to_wstring(std::string_view(s));
}

int string_width(std::string_view);

// Split the string into a its glyphs. An empty one is inserted ater fullwidth
// ones.
std::vector<std::string> Utf8ToGlyphs(std::string_view input);

// Map every cells drawn by |input| to their corresponding Glyphs. Half-size
// Glyphs takes one cell, full-size Glyphs take two cells.
std::vector<int> CellToGlyphIndex(std::string_view input);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_STRING_HPP */
