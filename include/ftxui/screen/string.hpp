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

int wchar_width(char32_t);
int wchar_width(wchar_t);
int wchar_width_cjk(char32_t);
int wchar_width_cjk(wchar_t);
int wstring_width(const std::wstring&);
int wstring_width_cjk(const std::wstring&);

/// @brief For convenience, this class convert multiple mutable string
/// references toward a shared representation.
class StringRef {
 public:
  StringRef(std::wstring* ref);
  StringRef(std::wstring ref);
  StringRef(const wchar_t* ref);
  StringRef(const char* ref);

  std::wstring& operator*();
  std::wstring* operator->();

 private:
  std::wstring* const borrowed_ = nullptr;
  std::wstring owned_;
};

/// @brief For convenience, this class convert multiple immutable string
/// references toward  shared representation.
class ConstStringRef {
 public:
  ConstStringRef(const std::wstring* ref);
  ConstStringRef(std::wstring ref);
  ConstStringRef(const wchar_t* ref);
  ConstStringRef(const char* ref);

  const std::wstring& operator*();
  const std::wstring* operator->();

 private:
  const std::wstring* const borrowed_ = nullptr;
  const std::wstring owned_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_STRING_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
