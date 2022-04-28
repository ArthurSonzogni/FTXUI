#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/component/component.hpp"          // for Radiobox
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for RadioboxOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown, Event::ArrowUp, Event::Tab, Event::TabReverse
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"    // for EXPECT_EQ, Test, TEST

namespace ftxui {

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

TEST(RadioboxTest, RemoveEntries) {
  int focused_entry = 0;
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  RadioboxOption option;
  option.focused_entry = &focused_entry;
  auto radiobox = Radiobox(&entries, &selected, option);

  EXPECT_EQ(selected, 0);
  EXPECT_EQ(focused_entry, 0);

  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::ArrowDown);
  radiobox->OnEvent(Event::Return);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  entries.resize(2);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  (void)radiobox->Render();

  EXPECT_EQ(selected, 1);
  EXPECT_EQ(focused_entry, 1);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
