#ifndef FTXUI_SCREEN_STRING_HPP
#define FTXUI_SCREEN_STRING_HPP

#include <string>

namespace ftxui {
std::string to_string(const std::wstring& s);
std::wstring to_wstring(const std::string& s);

template <typename T>
std::wstring to_wstring(T s) {
  return to_wstring(std::to_string(s));
}

int wchar_width(wchar_t);
int wchar_width_cjk(wchar_t);
int wstring_width(const std::wstring&);
int wstring_width_cjk(const std::wstring&);
}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_STRING_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
