// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_UTIL_REF_HPP
#define FTXUI_UTIL_REF_HPP

#include <ftxui/screen/string.hpp>
#include <string>
#include <variant>

namespace ftxui {

/// @brief An adapter. Own or reference an immutable object.
template <typename T>
class ConstRef {
 public:
  ConstRef() = default;
  ConstRef(const ConstRef<T>&) = default;
  ConstRef(ConstRef<T>&&) = default;
  ConstRef(T t) : variant_(std::move(t)) {}
  ConstRef(const T* t) : variant_(t) {}

  // Make a "reseatable" reference
  ConstRef<T>& operator=(const ConstRef<T>&) = default;

  // Accessors:
  const T& operator()() const { return *Address(); }
  const T& operator*() const { return *Address(); }
  const T* operator->() const { return Address(); }

 private:
  std::variant<T, const T*> variant_ = T{};

  const T* Address() const {
    return std::holds_alternative<T>(variant_) ? &std::get<T>(variant_)
                                               : std::get<const T*>(variant_);
  }
};

/// @brief An adapter. Own or reference an mutable object.
template <typename T>
class Ref {
 public:
  Ref() = default;
  Ref(const Ref<T>&) = default;
  Ref(Ref<T>&&) = default;
  Ref(T t) : variant_(std::move(t)) {}
  Ref(T* t) : variant_(t) {}

  // Make a "reseatable" reference.
  Ref<T>& operator=(const Ref<T>&) = default;

  // Accessors:
  T& operator()() { return *Address(); }
  T& operator*() { return *Address(); }
  T* operator->() { return Address(); }
  const T& operator()() const { return *Address(); }
  const T& operator*() const { return *Address(); }
  const T* operator->() const { return Address(); }

 private:
  std::variant<T, T*> variant_ = T{};

  const T* Address() const {
    return std::holds_alternative<T>(variant_) ? &std::get<T>(variant_)
                                               : std::get<T*>(variant_);
  }
  T* Address() {
    return std::holds_alternative<T>(variant_) ? &std::get<T>(variant_)
                                               : std::get<T*>(variant_);
  }
};

/// @brief An adapter. Own or reference a constant string. For convenience, this
/// class convert multiple mutable string toward a shared representation.
class StringRef : public Ref<std::string> {
 public:
  using Ref<std::string>::Ref;

  StringRef(const wchar_t* ref) : StringRef(to_string(std::wstring(ref))) {}
  StringRef(const char* ref) : StringRef(std::string(ref)) {}
};

/// @brief An adapter. Own or reference a constant string. For convenience, this
/// class convert multiple immutable string toward a shared representation.
class ConstStringRef : public ConstRef<std::string> {
 public:
  using ConstRef<std::string>::ConstRef;

  ConstStringRef(const std::wstring* ref) : ConstStringRef(to_string(*ref)) {}
  ConstStringRef(const std::wstring ref) : ConstStringRef(to_string(ref)) {}
  ConstStringRef(const wchar_t* ref)
      : ConstStringRef(to_string(std::wstring(ref))) {}
  ConstStringRef(const char* ref) : ConstStringRef(std::string(ref)) {}
};

/// @brief An adapter. Reference a list of strings.
class ConstStringListRef {
 public:
  ConstStringListRef() = default;
  ConstStringListRef(const std::vector<std::string>* ref) : ref_(ref) {}
  ConstStringListRef(const std::vector<std::wstring>* ref) : ref_wide_(ref) {}
  ConstStringListRef(const ConstStringListRef& other) = default;
  ConstStringListRef& operator=(const ConstStringListRef& other) = default;

  size_t size() const {
    if (ref_) {
      return ref_->size();
    }
    if (ref_wide_) {
      return ref_wide_->size();
    }
    return 0;
  }

  std::string operator[](size_t i) const {
    if (ref_) {
      return (*ref_)[i];
    }
    if (ref_wide_) {
      return to_string((*ref_wide_)[i]);
    }
    return "";
  }

 private:
  const std::vector<std::string>* ref_ = nullptr;
  const std::vector<std::wstring>* ref_wide_ = nullptr;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_UTIL_REF_HPP */
