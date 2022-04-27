#ifndef FTXUI_UTIL_REF_HPP
#define FTXUI_UTIL_REF_HPP

#include <ftxui/screen/string.hpp>
#include <string>

namespace ftxui {

/// @brief An adapter. Own or reference an immutable object.
template <typename T>
class ConstRef {
 public:
  ConstRef() {}
  ConstRef(T t) : owned_(t) {}
  ConstRef(const T* t) : address_(t) {}
  const T& operator*() const { return address_ ? *address_ : owned_; }
  const T& operator()() const { return address_ ? *address_ : owned_; }
  const T* operator->() const { return address_ ? address_ : &owned_; }

 private:
  T owned_;
  const T* address_ = nullptr;
};

/// @brief An adapter. Own or reference an mutable object.
template <typename T>
class Ref {
 public:
  Ref() {}
  Ref(const T& t) : owned_(t) {}
  Ref(T&& t) : owned_(std::forward<T>(t)) {}
  Ref(T* t) : address_(t) {}
  T& operator*() { return address_ ? *address_ : owned_; }
  T& operator()() { return address_ ? *address_ : owned_; }
  T* operator->() { return address_ ? address_ : &owned_; }

 private:
  T owned_;
  T* address_ = nullptr;
};

/// @brief An adapter. Own or reference a constant string. For convenience, this
/// class convert multiple mutable string toward a shared representation.
class StringRef {
 public:
  StringRef(std::string* ref) : address_(ref) {}
  StringRef(std::string ref) : owned_(std::move(ref)) {}
  StringRef(const wchar_t* ref) : StringRef(to_string(std::wstring(ref))) {}
  StringRef(const char* ref) : StringRef(std::string(ref)) {}
  std::string& operator*() { return address_ ? *address_ : owned_; }
  std::string* operator->() { return address_ ? address_ : &owned_; }

 private:
  std::string owned_;
  std::string* address_ = nullptr;
};

/// @brief An adapter. Own or reference a constant string. For convenience, this
/// class convert multiple immutable string toward a shared representation.
class ConstStringRef {
 public:
  ConstStringRef(const std::string* ref) : address_(ref) {}
  ConstStringRef(const std::wstring* ref) : ConstStringRef(to_string(*ref)) {}
  ConstStringRef(std::string ref) : owned_(std::move(ref)) {}
  ConstStringRef(std::wstring ref) : ConstStringRef(to_string(ref)) {}
  ConstStringRef(const wchar_t* ref) : ConstStringRef(std::wstring(ref)) {}
  ConstStringRef(const char* ref)
      : ConstStringRef(to_wstring(std::string(ref))) {}
  const std::string& operator*() const { return address_ ? *address_ : owned_; }
  const std::string* operator->() const {
    return address_ ? address_ : &owned_;
  }

 private:
  const std::string owned_;
  const std::string* address_ = nullptr;
};

/// @brief An adapter. Reference a list of strings.
class ConstStringListRef {
 public:
  ConstStringListRef(const std::vector<std::string>* ref) : ref_(ref) {}
  ConstStringListRef(const std::vector<std::wstring>* ref) : ref_wide_(ref) {}

  size_t size() const { return ref_ ? ref_->size() : ref_wide_->size(); }
  std::string operator[](size_t i) const {
    return ref_ ? (*ref_)[i] : to_string((*ref_wide_)[i]);
  }

 private:
  const std::vector<std::string>* ref_ = nullptr;
  const std::vector<std::wstring>* ref_wide_ = nullptr;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_UTIL_REF_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
