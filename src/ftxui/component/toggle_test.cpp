#include "ftxui/component/toggle.hpp"

#include "gtest/gtest.h"

using namespace ftxui;

TEST(ToggleTest, leftRightArrow) {
  Toggle toggle;

  // With arrow key.
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::ArrowRight);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::ArrowRight);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(toggle.selected, 0);

  // With vim like characters.
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::Character('l'));
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::Character('l'));
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::Character('h'));
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::Character('h'));
  EXPECT_EQ(toggle.selected, 0);

  // With more entries
  toggle.entries = {L"1", L"2", L"3"};
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::ArrowRight);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::ArrowRight);
  EXPECT_EQ(toggle.selected, 2);
  toggle.OnEvent(Event::ArrowRight);
  EXPECT_EQ(toggle.selected, 2);
  toggle.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(toggle.selected, 0);
}

TEST(ToggleTest, Tab) {
  Toggle toggle;
  toggle.entries = {L"1", L"2", L"3"};

  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::Tab);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::Tab);
  EXPECT_EQ(toggle.selected, 2);
  toggle.OnEvent(Event::Tab);
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::Tab);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::Tab);
  EXPECT_EQ(toggle.selected, 2);
  toggle.OnEvent(Event::TabReverse);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::TabReverse);
  EXPECT_EQ(toggle.selected, 0);
  toggle.OnEvent(Event::TabReverse);
  EXPECT_EQ(toggle.selected, 2);
  toggle.OnEvent(Event::TabReverse);
  EXPECT_EQ(toggle.selected, 1);
  toggle.OnEvent(Event::TabReverse);
}

TEST(ToggleTest, OnChange) {
  Toggle toggle;
  toggle.entries = {L"1", L"2", L"3"};

  int counter = 0;
  toggle.on_change = [&] { counter++; };

  EXPECT_FALSE(toggle.OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_EQ(counter, 0);

  EXPECT_TRUE(toggle.OnEvent(Event::ArrowRight));  // [0] -> [1]
  EXPECT_EQ(counter, 1);
  EXPECT_TRUE(toggle.OnEvent(Event::ArrowRight));  // [1] -> [2]
  EXPECT_EQ(counter, 2);

  EXPECT_FALSE(toggle.OnEvent(Event::ArrowRight));  // Reached far right.
  EXPECT_EQ(counter, 2);

  EXPECT_TRUE(toggle.OnEvent(Event::ArrowLeft));  // [2] -> [1]
  EXPECT_EQ(counter, 3);
  EXPECT_TRUE(toggle.OnEvent(Event::ArrowLeft));  // [1] -> [0]
  EXPECT_EQ(counter, 4);

  EXPECT_FALSE(toggle.OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_EQ(counter, 4);
}

TEST(ToggleTest, OnEnter) {
  Toggle toggle;
  toggle.entries = {L"1", L"2", L"3"};

  int counter = 0;
  toggle.on_enter = [&] { counter++; };

  EXPECT_FALSE(toggle.OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 1);

  EXPECT_TRUE(toggle.OnEvent(Event::ArrowRight));  // [0] -> [1]
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 2);
  EXPECT_TRUE(toggle.OnEvent(Event::ArrowRight));  // [1] -> [2]
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 3);

  EXPECT_FALSE(toggle.OnEvent(Event::ArrowRight));  // Reached far right.
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 4);

  EXPECT_TRUE(toggle.OnEvent(Event::ArrowLeft));  // [2] -> [1]
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 5);
  EXPECT_TRUE(toggle.OnEvent(Event::ArrowLeft));  // [1] -> [0]
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 6);

  EXPECT_FALSE(toggle.OnEvent(Event::ArrowLeft));  // Reached far left.
  EXPECT_TRUE(toggle.OnEvent(Event::Return));
  EXPECT_EQ(counter, 7);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
