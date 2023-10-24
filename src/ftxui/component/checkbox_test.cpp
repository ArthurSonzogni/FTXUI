// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <chrono>  // for operator""s, chrono_literals
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string

#include "ftxui/component/component.hpp"          // for Button, Horizontal
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowLeft, Event::ArrowRight
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen
#include "ftxui/screen/terminal.hpp"  // for SetColorSupport, Color, TrueColor
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, Test, EXPECT_FALSE, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

using namespace std::chrono_literals;

TEST(CheckboxTest, Basic) {
  bool checked1 = false;
  bool checked2 = false;
  auto checkbox1 = Checkbox("Checkbox 1", &checked1);
  auto checkbox2 = Checkbox("Checkbox 2", &checked2);

  int selected = 0;
  auto container = Container::Horizontal(
      {
          checkbox1,
          checkbox2
      },
      &selected);

  (void)container->Render();

  EXPECT_FALSE(checked1);
  EXPECT_FALSE(checked2);
  EXPECT_TRUE(checkbox1->Focused());
  EXPECT_FALSE(checkbox2->Focused());

  container->OnEvent(Event::ArrowLeft);

  EXPECT_FALSE(checked1);
  EXPECT_FALSE(checked2);
  EXPECT_TRUE(checkbox1->Focused());
  EXPECT_FALSE(checkbox2->Focused());

  container->OnEvent(Event::ArrowRight);

  EXPECT_FALSE(checked1);
  EXPECT_FALSE(checked2);
  EXPECT_FALSE(checkbox1->Focused());
  EXPECT_TRUE(checkbox2->Focused());

  container->OnEvent(Event::ArrowRight);

  EXPECT_FALSE(checked1);
  EXPECT_FALSE(checked2);
  EXPECT_FALSE(checkbox1->Focused());
  EXPECT_TRUE(checkbox2->Focused());

  container->OnEvent(Event::Return);

  EXPECT_FALSE(checked1);
  EXPECT_TRUE(checked2);
  EXPECT_FALSE(checkbox1->Focused());
  EXPECT_TRUE(checkbox2->Focused());

  container->OnEvent(Event::Return);

  EXPECT_FALSE(checked1);
  EXPECT_FALSE(checked2);
  EXPECT_FALSE(checkbox1->Focused());
  EXPECT_TRUE(checkbox2->Focused());

  container->OnEvent(Event::ArrowLeft);
  container->OnEvent(Event::Return);

  EXPECT_TRUE(checked1);
  EXPECT_FALSE(checked2);
  EXPECT_TRUE(checkbox1->Focused());
  EXPECT_FALSE(checkbox2->Focused());
}

TEST(CheckboxTest, DragClick) {
  bool checked1 = false;
  auto checkbox1 = Checkbox("Checkbox 1", &checked1);

  auto container = Container::Horizontal(
      {
          checkbox1
      });

  (void)container->Render();

  Event leftMousePress = Event::Mouse("", {Mouse::Button::Left, Mouse::Motion::Pressed});
  Event leftMouseRelease = Event::Mouse("", {Mouse::Button::Left, Mouse::Motion::Released});

  EXPECT_FALSE(checked1);

  checkbox1->OnEvent(leftMousePress);

  EXPECT_TRUE(checked1);

  checkbox1->OnEvent(leftMousePress);
  checkbox1->OnEvent(leftMousePress);
  checkbox1->OnEvent(leftMousePress);

  EXPECT_TRUE(checked1);

  checkbox1->OnEvent(leftMouseRelease);

  EXPECT_TRUE(checked1);

  checkbox1->OnEvent(leftMousePress);

  EXPECT_FALSE(checked1);

  checkbox1->OnEvent(leftMouseRelease);

  EXPECT_FALSE(checked1);
}

}  // namespace ftxui
// NOLINTEND
