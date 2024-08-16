// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/util/ref.hpp"

#include <gtest/gtest.h>
#include "ftxui/component/component.hpp"

namespace ftxui {
namespace {
class Adapter : public ConstStringListRef::Adapter {
 public:
  Adapter(std::vector<std::string>& entries) : entries(entries) {}
  size_t size() const override { return entries.size() * 2; }
  std::string operator[](size_t index) const override {
    return entries[index / 2];
  }
  std::vector<std::string>& entries;
};
}  // namespace

TEST(ConstStringListRef, Copy) {
  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };
  int selected = 0;
  auto menu = Menu(entries, &selected);
}

TEST(ConstStringListRef, Ref) {
  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };
  int selected = 0;
  auto menu = Menu(&entries, &selected);
}

TEST(ConstStringListRef, Adapter) {
  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };

  int selected = 0;
  Adapter a(entries);
  auto menu = Menu(&a, &selected);
}

TEST(ConstStringListRef, UniquePtrAdapter) {
  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };

  int selected = 0;
  auto a = std::make_unique<Adapter>(entries);
  auto menu = Menu(std::move(a), &selected);
}

}  // namespace ftxui
