#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult
#include <memory>  // for shared_ptr, __shared_ptr_access, allocator, make_shared

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "gtest/gtest_pred_impl.h"  // for EXPECT_EQ, Test, SuiteApiResolver, TEST, TestFactoryImpl

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

TEST(ContainerTest, Detach) {
  auto parent = Make();
  auto child_1 = Make();
  auto child_2 = Make();
  auto child_3 = Make();
  parent->Add(child_1);
  parent->Add(child_2);
  parent->Add(child_3);

  EXPECT_EQ(parent->ChildCount(), 3u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), parent.get());
  EXPECT_EQ(child_3->Parent(), parent.get());

  child_2->Detach();

  EXPECT_EQ(parent->ChildCount(), 2u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), nullptr);
  EXPECT_EQ(child_3->Parent(), parent.get());

  child_2->Detach();

  EXPECT_EQ(parent->ChildCount(), 2u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), nullptr);
  EXPECT_EQ(child_3->Parent(), parent.get());

  child_1->Detach();

  EXPECT_EQ(parent->ChildCount(), 1u);
  EXPECT_EQ(child_1->Parent(), nullptr);
  EXPECT_EQ(child_2->Parent(), nullptr);
  EXPECT_EQ(child_3->Parent(), parent.get());

  child_3->Detach();

  EXPECT_EQ(parent->ChildCount(), 0u);
  EXPECT_EQ(child_1->Parent(), nullptr);
  EXPECT_EQ(child_2->Parent(), nullptr);
  EXPECT_EQ(child_3->Parent(), nullptr);
}

TEST(ContainerTest, DetachAllChildren) {
  auto parent = Make();
  auto child_1 = Make();
  auto child_2 = Make();
  auto child_3 = Make();
  parent->Add(child_1);
  parent->Add(child_2);
  parent->Add(child_3);

  EXPECT_EQ(parent->ChildCount(), 3u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), parent.get());
  EXPECT_EQ(child_3->Parent(), parent.get());

  parent->DetachAllChildren();

  EXPECT_EQ(parent->ChildCount(), 0u);
  EXPECT_EQ(child_1->Parent(), nullptr);
  EXPECT_EQ(child_2->Parent(), nullptr);
  EXPECT_EQ(child_3->Parent(), nullptr);
}

TEST(ContainerTest, Add) {
  auto parent = Make();
  auto child_1 = Make();
  auto child_2 = Make();

  EXPECT_EQ(parent->ChildCount(), 0u);
  EXPECT_EQ(child_1->Parent(), nullptr);
  EXPECT_EQ(child_2->Parent(), nullptr);

  parent->Add(child_1);

  EXPECT_EQ(parent->ChildCount(), 1u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), nullptr);

  parent->Add(child_1);

  EXPECT_EQ(parent->ChildCount(), 1u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), nullptr);

  parent->Add(child_2);

  EXPECT_EQ(parent->ChildCount(), 2u);
  EXPECT_EQ(child_1->Parent(), parent.get());
  EXPECT_EQ(child_2->Parent(), parent.get());
}

TEST(ContainerTest, ChildAt) {
  auto parent = Make();
  auto child_1 = Make();
  auto child_2 = Make();

  EXPECT_EQ(parent->ChildCount(), 0u);

  parent->Add(child_1);

  EXPECT_EQ(parent->ChildCount(), 1u);
  EXPECT_EQ(parent->ChildAt(0u), child_1);

  parent->Add(child_2);

  EXPECT_EQ(parent->ChildCount(), 2u);
  EXPECT_EQ(parent->ChildAt(0u), child_1);
  EXPECT_EQ(parent->ChildAt(1u), child_2);

  parent->Add(child_1);

  EXPECT_EQ(parent->ChildCount(), 2u);
  EXPECT_EQ(parent->ChildAt(0u), child_2);
  EXPECT_EQ(parent->ChildAt(1u), child_1);

  child_1->Detach();

  EXPECT_EQ(parent->ChildCount(), 1u);
  EXPECT_EQ(parent->ChildAt(0u), child_2);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
