#include "ftxui/component/radiobox.hpp"

#include "gtest/gtest.h"

using namespace ftxui;

TEST(RadioboxTest, Navigation) {
  RadioBox radiobox;
  radiobox.entries = {L"1", L"2", L"3"};

  // With arrow key.
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::ArrowDown);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::ArrowDown);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::ArrowDown);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::ArrowUp);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::ArrowUp);
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::ArrowUp);
  EXPECT_EQ(radiobox.focused, 0);

  // With vim like characters.
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::Character('j'));
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::Character('j'));
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::Character('j'));
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::Character('k'));
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::Character('k'));
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::Character('k'));
  EXPECT_EQ(radiobox.focused, 0);

  // With more entries
  radiobox.entries = {L"1", L"2", L"3"};
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::ArrowDown);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::ArrowDown);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::ArrowDown);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::ArrowUp);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::ArrowUp);
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::ArrowUp);
  EXPECT_EQ(radiobox.focused, 0);

  // With tab.
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::Tab);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::Tab);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::Tab);
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::Tab);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::Tab);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::TabReverse);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::TabReverse);
  EXPECT_EQ(radiobox.focused, 0);
  radiobox.OnEvent(Event::TabReverse);
  EXPECT_EQ(radiobox.focused, 2);
  radiobox.OnEvent(Event::TabReverse);
  EXPECT_EQ(radiobox.focused, 1);
  radiobox.OnEvent(Event::TabReverse);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
