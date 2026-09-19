// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/component.hpp"  // for Horizontal, Vertical, Button, Tab
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"  // for Event, Event::Tab, Event::TabReverse, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_FALSE, Test, EXPECT_TRUE, TEST

namespace ftxui {

namespace {
Event MousePressed(int x, int y) {
  Mouse mouse;
  mouse.button = Mouse::Left;
  mouse.motion = Mouse::Pressed;
  mouse.x = x;
  mouse.y = y;
  return Event::Mouse("", mouse);
}
}  // namespace

TEST(DropdownTest, Empty) {
  std::vector<std::string> list = {};
  int index = 0;
  auto dropdown = Dropdown(list, &index);

  dropdown->OnEvent(Event::Return);

  auto screen = Screen(8, 8);
  auto document = dropdown->Render();
  Render(screen, document);

  EXPECT_EQ(screen.ToString(),
            "╭──────╮\r\n"
            "│↓     │\r\n"
            "├──────┤\r\n"
            "│      │\r\n"
            "│      │\r\n"
            "│      │\r\n"
            "│      │\r\n"
            "╰──────╯");
}

// https://github.com/ArthurSonzogni/FTXUI/issues/1278
TEST(DropdownTest, ClosesWhenAnotherDropdownTakesFocus) {
  std::vector<std::string> entries = {"a", "b", "c"};
  int top_selected = 0;
  int bottom_selected = 0;
  bool top_open = false;
  bool bottom_open = false;

  DropdownOption top_option;
  top_option.radiobox.entries = &entries;
  top_option.radiobox.selected = &top_selected;
  top_option.checkbox.checked = &top_open;
  DropdownOption bottom_option = top_option;
  bottom_option.radiobox.selected = &bottom_selected;
  bottom_option.checkbox.checked = &bottom_open;

  auto layout = Container::Vertical({
      Dropdown(top_option),
      Dropdown(bottom_option),
  });

  auto screen = Screen(8, 12);
  auto render = [&] { Render(screen, layout->Render()); };

  // Open the bottom dropdown.
  render();
  layout->OnEvent(MousePressed(2, 4));
  render();
  EXPECT_FALSE(top_open);
  EXPECT_TRUE(bottom_open);

  // Open the top dropdown. The bottom one must close.
  layout->OnEvent(MousePressed(2, 1));
  render();
  EXPECT_TRUE(top_open);
  EXPECT_FALSE(bottom_open);

  // Close the top dropdown, move to the bottom one, and reopen it with the
  // keyboard. Focus must be back on its checkbox.
  layout->OnEvent(Event::Escape);
  render();
  EXPECT_FALSE(top_open);
  layout->OnEvent(Event::ArrowDown);
  render();
  layout->OnEvent(Event::Return);
  render();
  EXPECT_FALSE(top_open);
  EXPECT_TRUE(bottom_open);
}

}  // namespace ftxui
