// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/screen/string.hpp"

#include <codecvt>
#include <locale>

namespace ftxui {
#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4996)  // codecvt_utf8_utf16 is deprecated
#endif

std::string to_string(const std::wstring& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(s);
}

std::wstring to_wstring(const std::string& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(s);
}

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

}  // namespace ftxui
