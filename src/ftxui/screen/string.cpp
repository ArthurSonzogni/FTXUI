#include "ftxui/screen/string.hpp"

#include <codecvt>  // for codecvt_utf8_utf16
#include <locale>   // for wstring_convert
#include <utility>  // for move

namespace ftxui {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)  // codecvt_utf8_utf16 is deprecated
#endif

/// Convert a UTF8 std::string into a std::wstring.
std::string to_string(const std::wstring& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(s);
}

/// Convert a std::wstring into a UTF8 std::string.
std::wstring to_wstring(const std::string& s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(s);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

StringRef::StringRef(std::wstring* ref) : borrowed_(ref) {}
StringRef::StringRef(std::wstring ref) : owned_(std::move(ref)) {}
StringRef::StringRef(const wchar_t* ref) : owned_(ref) {}
StringRef::StringRef(const char* ref) : owned_(to_wstring(std::string(ref))) {}
std::wstring& StringRef::operator*() {
  return borrowed_ ? *borrowed_ : owned_;
}
std::wstring* StringRef::operator->() {
  return borrowed_ ? borrowed_ : &owned_;
}

ConstStringRef::ConstStringRef(const std::wstring* ref) : borrowed_(ref) {}
ConstStringRef::ConstStringRef(std::wstring ref) : owned_(std::move(ref)) {}
ConstStringRef::ConstStringRef(const wchar_t* ref) : owned_(ref) {}
ConstStringRef::ConstStringRef(const char* ref)
    : owned_(to_wstring(std::string(ref))) {}

const std::wstring& ConstStringRef::operator*() {
  return borrowed_ ? *borrowed_ : owned_;
}
const std::wstring* ConstStringRef::operator->() {
  return borrowed_ ? borrowed_ : &owned_;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
