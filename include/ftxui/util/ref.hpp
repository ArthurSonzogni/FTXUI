#ifndef FTXUI_UTIL_REF_HPP
#define FTXUI_UTIL_REF_HPP

#include <ftxui/screen/string.hpp>
#include <string>

namespace ftxui {

/// @brief An adapter. Own or reference a constant object.
template <typename T>
class ConstRef {
 public:
  ConstRef() {}
  ConstRef(T t) : owned_(t) {}
  ConstRef(const T* t) : address_(t) {}
  const T& operator*() { return address_ ? *address_ : owned_; }
  const T* operator->() { return address_ ? address_ : &owned_; }

 private:
  T owned_;
  const T* address_ = nullptr;
};

/// @brief An adapter. Own or reference a constant string. For convenience, this
/// class convert multiple mutable string toward a shared representation.
class StringRef {
 public:
  StringRef(std::wstring* ref) : address_(ref) {}
  StringRef(std::wstring ref) : owned_(std::move(ref)) {}
  StringRef(const wchar_t* ref) : StringRef(std::wstring(ref)) {}
  StringRef(const char* ref) : StringRef(to_wstring(std::string(ref))) {}
  std::wstring& operator*() { return address_ ? *address_ : owned_; }
  std::wstring* operator->() { return address_ ? address_ : &owned_; }

 private:
  std::wstring owned_;
  std::wstring* address_ = nullptr;
};

/// @brief An adapter. Own or reference a constant string. For convenience, this
/// class convert multiple immutable string toward a shared representation.
class ConstStringRef {
 public:
  ConstStringRef(const std::wstring* ref) : address_(ref) {}
  ConstStringRef(std::wstring ref) : owned_(std::move(ref)) {}
  ConstStringRef(const wchar_t* ref) : ConstStringRef(std::wstring(ref)) {}
  ConstStringRef(const char* ref)
      : ConstStringRef(to_wstring(std::string(ref))) {}
  const std::wstring& operator*() { return address_ ? *address_ : owned_; }
  const std::wstring* operator->() { return address_ ? address_ : &owned_; }

 private:
  const std::wstring owned_;
  const std::wstring* address_ = nullptr;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_UTIL_REF_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
