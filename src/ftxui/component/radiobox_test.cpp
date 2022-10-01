#include <gtest/gtest.h>
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/component/component.hpp"          // for Radiobox
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for RadioboxOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown, Event::ArrowUp, Event::Tab, Event::TabReverse
#include "ftxui/util/ref.hpp"         // for Ref

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

TEST(RadioboxTest, EventHandling) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2"};
  auto radiobox = Radiobox(&entries, &selected);
  ASSERT_EQ(selected, 0);

  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_FALSE(radiobox->OnEvent(Event::ArrowDown))
      << "Should not handle ArrowDown when we are on last item";
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowUp));
  EXPECT_FALSE(radiobox->OnEvent(Event::ArrowUp))
      << "Should not handle ArrowUp when we are on first item";

  EXPECT_TRUE(radiobox->OnEvent(Event::End));
  EXPECT_FALSE(radiobox->OnEvent(Event::End))
      << "Should not handle End when we are on last item";
  EXPECT_TRUE(radiobox->OnEvent(Event::Home));
  EXPECT_FALSE(radiobox->OnEvent(Event::Home))
      << "Should not handle Home when we are on first item";

  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_TRUE(radiobox->OnEvent(Event::Character(' ')));
  ASSERT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character(' ')))
      << "Should handle Space even when focused item is already selected";

  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowUp));
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  ASSERT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return))
      << "Should handle Return even when focused item is already selected";
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
