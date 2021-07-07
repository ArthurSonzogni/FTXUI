#include <memory>  // for shared_ptr, allocator, make_shared, __shared_ptr_access

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "gtest/gtest_pred_impl.h"  // for Test, SuiteApiResolver, TEST, TestFactoryImpl

using namespace ftxui;

namespace {
Component Make() {
  return std::make_shared<ComponentBase>();
}
}  // namespace

// Regression test for:
// https://github.com/ArthurSonzogni/FTXUI/issues/115
TEST(ContainerTest, DeleteParentFirst) {
  auto parent = Make();
  auto child = Make();
  parent->Add(child);
  parent.reset();
  child.reset();
}

TEST(ContainerTest, DeleteChildFirst) {
  auto parent = Make();
  auto child = Make();
  parent->Add(child);
  child.reset();
  parent.reset();
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
