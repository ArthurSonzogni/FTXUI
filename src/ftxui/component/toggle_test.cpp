// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <functional>  // for function
#include <string>      // for string, basic_string
#include <vector>      // for vector

#include "ftxui/component/component.hpp"          // for Menu, Toggle
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, Test, EXPECT_TRUE, EXPECT_FALSE, TEST

// NOLINTBEGIN
namespace ftxui {

TEST(ToggleTest, leftRightArrow) {
  std::vector<std::string> entries = {"On", "Off"};
  int selected = 0;
  auto toggle = Toggle(&entries, &selected);

  // With arrow key.
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);

  // With vim like characters.
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::Character('l'));
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::Character('l'));
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::Character('h'));
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::Character('h'));
  EXPECT_EQ(selected, 0);

  // With more entries
  entries = {"1", "2", "3"};
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 2);
  toggle->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 2);
  toggle->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);
}

TEST(ToggleTest, Tab) {
  std::vector<std::string> entries = {"1", "2", "3"};
  int selected = 0;
  auto toggle = Toggle(&entries, &selected);

  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::Tab);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::Tab);
  EXPECT_EQ(selected, 2);
  toggle->OnEvent(Event::Tab);
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::Tab);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::Tab);
  EXPECT_EQ(selected, 2);
  toggle->OnEvent(Event::TabReverse);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::TabReverse);
  EXPECT_EQ(selected, 0);
  toggle->OnEvent(Event::TabReverse);
  EXPECT_EQ(selected, 2);
  toggle->OnEvent(Event::TabReverse);
  EXPECT_EQ(selected, 1);
  toggle->OnEvent(Event::TabReverse);
}

TEST(ToggleTest, OnChange) {
  std::vector<std::string> entries = {"1", "2", "3"};
  int selected = 0;
  int counter = 0;
  auto option = MenuOption::Toggle();
  option.on_change = [&] { counter++; };

  auto toggle = Menu(&entries, &selected, option);

  EXPECT_FALSE(toggle->OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_EQ(counter, 0);

  EXPECT_TRUE(toggle->OnEvent(Event::ArrowRight));  // [0] -> [1]
  EXPECT_EQ(counter, 1);
  EXPECT_TRUE(toggle->OnEvent(Event::ArrowRight));  // [1] -> [2]
  EXPECT_EQ(counter, 2);

  EXPECT_FALSE(toggle->OnEvent(Event::ArrowRight));  // Reached far right.
  EXPECT_EQ(counter, 2);

  EXPECT_TRUE(toggle->OnEvent(Event::ArrowLeft));  // [2] -> [1]
  EXPECT_EQ(counter, 3);
  EXPECT_TRUE(toggle->OnEvent(Event::ArrowLeft));  // [1] -> [0]
  EXPECT_EQ(counter, 4);

  EXPECT_FALSE(toggle->OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_EQ(counter, 4);
}

TEST(ToggleTest, OnEnter) {
  std::vector<std::string> entries = {"1", "2", "3"};
  int selected = 0;
  int counter = 0;

  auto option = MenuOption::Toggle();
  option.on_enter = [&] { counter++; };
  auto toggle = Menu(&entries, &selected, option);

  EXPECT_FALSE(toggle->OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 1);

  EXPECT_TRUE(toggle->OnEvent(Event::ArrowRight));  // [0] -> [1]
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 2);
  EXPECT_TRUE(toggle->OnEvent(Event::ArrowRight));  // [1] -> [2]
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 3);

  EXPECT_FALSE(toggle->OnEvent(Event::ArrowRight));  // Reached far right.
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 4);

  EXPECT_TRUE(toggle->OnEvent(Event::ArrowLeft));  // [2] -> [1]
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 5);
  EXPECT_TRUE(toggle->OnEvent(Event::ArrowLeft));  // [1] -> [0]
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 6);

  EXPECT_FALSE(toggle->OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_TRUE(toggle->OnEvent(Event::Return));
  EXPECT_EQ(counter, 7);
}

TEST(ToggleTest, RemoveEntries) {
  int focused_entry = 0;
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto option = MenuOption::Toggle();
  option.focused_entry = &focused_entry;
  auto toggle = Menu(&entries, &selected, option);

  EXPECT_EQ(selected, 0);
  EXPECT_EQ(focused_entry, 0);

  toggle->OnEvent(Event::ArrowRight);
  toggle->OnEvent(Event::ArrowRight);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  entries.resize(2);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  (void)toggle->Render();

  EXPECT_EQ(selected, 1);
  EXPECT_EQ(focused_entry, 1);
}

}  // namespace ftxui
// NOLINTEND
