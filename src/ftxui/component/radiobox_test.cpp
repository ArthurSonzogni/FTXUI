#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Radiobox
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown, Event::ArrowUp, Event::Tab, Event::TabReverse
#include "gtest/gtest_pred_impl.h"    // for EXPECT_EQ, Test, TEST

using namespace ftxui;

TEST(RadioboxTest, Navigation) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  // With arrow key.
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::ArrowUp);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::ArrowUp);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::ArrowUp);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);

  // With vim like characters.
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::Character('j'));
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::Character('j'));
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::Character('j'));
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::Character('k'));
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::Character('k'));
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::Character('k'));
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);

  // With more entries
  entries = {"1", "2", "3"};
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::ArrowUp);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::ArrowUp);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::ArrowUp);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);

  // With tab.
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::Tab);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::Tab);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::Tab);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::Tab);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::Tab);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::TabReverse);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::TabReverse);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 0);
  radiobox->OnEvent(Event::TabReverse);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 2);
  radiobox->OnEvent(Event::TabReverse);
  radiobox->OnEvent(Event::Return);
  EXPECT_EQ(selected, 1);
  radiobox->OnEvent(Event::TabReverse);
  radiobox->OnEvent(Event::Return);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
