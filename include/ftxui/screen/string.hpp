// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_STRING_HPP
#define FTXUI_SCREEN_STRING_HPP

#include <string>       // for string, wstring, to_string
#include <string_view>  // for string_view
#include <vector>       // for vector

#include "ftxui/util/export.hpp"

namespace ftxui {
/// Convert a wide string to UTF-8.
FTXUI_EXPORT(SCREEN) std::string to_string(std::wstring_view s);
/// Convert a UTF-8 string to a wide string.
FTXUI_EXPORT(SCREEN) std::wstring to_wstring(std::string_view s);

/// Convert a value to a wide string via std::to_string.
template <typename T>
std::wstring to_wstring(T s) {
  return to_wstring(std::string_view(std::to_string(s)));
}
/// Convert a UTF-8 std::string to a wide string.
inline std::wstring to_wstring(const std::string& s) {
  return to_wstring(std::string_view(s));
}
/// Convert a null-terminated UTF-8 C-string to a wide string.
template <>
inline std::wstring to_wstring(const char* s) {
  return to_wstring(std::string_view(s));
}

/// Return the display width (in terminal columns) of a UTF-8 string.
FTXUI_EXPORT(SCREEN) int string_width(std::string_view);

/// Split a UTF-8 string into glyphs (inserting "" after fullwidth glyphs).
FTXUI_EXPORT(SCREEN)
std::vector<std::string> Utf8ToGlyphs(std::string_view input);

/// Map each terminal column of `input` to its glyph index.
FTXUI_EXPORT(SCREEN)
std::vector<int> CellToGlyphIndex(std::string_view input);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_STRING_HPP */
