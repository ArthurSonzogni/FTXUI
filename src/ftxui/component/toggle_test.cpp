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
