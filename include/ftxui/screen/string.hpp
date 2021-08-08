#ifndef FTXUI_SCREEN_STRING_HPP
#define FTXUI_SCREEN_STRING_HPP

#include <string>  // for string, wstring, to_string
#include <vector>  // for vector

namespace ftxui {
std::string to_string(const std::wstring& s);
std::wstring to_wstring(const std::string& s);

template <typename T>
std::wstring to_wstring(T s) {
  return to_wstring(std::to_string(s));
}

int string_width(const std::string&);
std::vector<std::string> Utf8ToGlyphs(const std::string& input);

}  // namespace ftxui

#include "ftxui/screen/deprecated.hpp"

#endif /* end of include guard: FTXUI_SCREEN_STRING_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
