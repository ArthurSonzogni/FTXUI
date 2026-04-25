// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for string

#include "ftxui/component/component.hpp"          // for Checkbox
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for CheckboxOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::Character
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_FALSE, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

namespace {

Event MousePressed(int x, int y) {
  Mouse mouse;
  mouse.button = Mouse::Left;
  mouse.motion = Mouse::Pressed;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;
  mouse.x = x;
  mouse.y = y;
  return Event::Mouse("jjj", mouse);
}

}  // namespace

TEST(CheckboxTest, Basic) {
  bool checked = false;
  std::string label = "option";
  auto checkbox = Checkbox(&label, &checked);

  EXPECT_FALSE(checked);
  EXPECT_TRUE(checkbox->Focused());

  // Space key toggles the checkbox.
  checkbox->OnEvent(Event::Character(' '));
  EXPECT_TRUE(checked);

  checkbox->OnEvent(Event::Character(' '));
  EXPECT_FALSE(checked);
}

TEST(CheckboxTest, ReturnKey) {
  bool checked = false;
  std::string label = "option";
  auto checkbox = Checkbox(&label, &checked);

  EXPECT_FALSE(checked);

  // Return key toggles the checkbox.
  checkbox->OnEvent(Event::Return);
  EXPECT_TRUE(checked);

  checkbox->OnEvent(Event::Return);
  EXPECT_FALSE(checked);
}

TEST(CheckboxTest, OnChange) {
  bool checked = false;
  int change_count = 0;
  std::string label = "option";
  CheckboxOption option;
  option.label = &label;
  option.checked = &checked;
  option.on_change = [&] { change_count++; };
  auto checkbox = Checkbox(option);

  EXPECT_EQ(change_count, 0);
  checkbox->OnEvent(Event::Character(' '));
  EXPECT_EQ(change_count, 1);
  EXPECT_TRUE(checked);

  checkbox->OnEvent(Event::Character(' '));
  EXPECT_EQ(change_count, 2);
  EXPECT_FALSE(checked);
}

TEST(CheckboxTest, MouseClick) {
  bool checked = false;
  std::string label = "option";
  auto checkbox = Checkbox(&label, &checked);

  // Render first to establish the box position.
  Screen screen(20, 1);
  Render(screen, checkbox->Render());

  EXPECT_FALSE(checked);
  checkbox->OnEvent(MousePressed(0, 0));
  EXPECT_TRUE(checked);

  checkbox->OnEvent(MousePressed(0, 0));
  EXPECT_FALSE(checked);
}

TEST(CheckboxTest, Focusable) {
  bool checked = false;
  std::string label = "option";
  auto checkbox = Checkbox(&label, &checked);
  EXPECT_TRUE(checkbox->Focusable());
}

TEST(CheckboxTest, Render) {
  bool checked = false;
  std::string label = "option";
  auto checkbox = Checkbox(&label, &checked);

  Screen screen(20, 1);
  Render(screen, checkbox->Render());
  std::string output = screen.ToString();
  // Output should contain the label.
  EXPECT_NE(output.find("option"), std::string::npos);
}

TEST(CheckboxTest, OptionSyntax) {
  bool checked = true;
  CheckboxOption option;
  std::string label = "item";
  option.label = &label;
  option.checked = &checked;
  auto checkbox = Checkbox(option);

  EXPECT_TRUE(checked);
  checkbox->OnEvent(Event::Character(' '));
  EXPECT_FALSE(checked);
}

}  // namespace ftxui
// NOLINTEND
