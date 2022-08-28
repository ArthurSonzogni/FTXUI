#include <gtest/gtest.h>  // for AssertionResult, Message, TestPartResult, Test, EXPECT_TRUE, EXPECT_EQ, SuiteApiResolver, TestInfo (ptr only), EXPECT_FALSE, TEST, TestFactoryImpl
#include <ftxui/component/mouse.hpp>  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include <ftxui/dom/elements.hpp>  // for GaugeDirection, GaugeDirection::Down, GaugeDirection::Left, GaugeDirection::Right, GaugeDirection::Up
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator

#include "ftxui/component/component.hpp"       // for Slider
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/dom/node.hpp"                  // for Render
#include "ftxui/screen/screen.hpp"             // for Screen

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

Event MouseReleased(int x, int y) {
  Mouse mouse;
  mouse.button = Mouse::Left;
  mouse.motion = Mouse::Released;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;
  mouse.x = x;
  mouse.y = y;
  return Event::Mouse("jjj", mouse);
}
}  // namespace

TEST(SliderTest, Right) {
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = GaugeDirection::Right,
  });
  Screen screen(11, 1);
  Render(screen, slider->Render());
  EXPECT_TRUE(slider->OnEvent(MousePressed(3, 0)));
  EXPECT_EQ(value, 30);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 0)));
  EXPECT_EQ(value, 90);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 2)));
  EXPECT_EQ(value, 90);
  EXPECT_TRUE(slider->OnEvent(MousePressed(5, 2)));
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(5, 2)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(5, 2)));
}

TEST(SliderTest, Left) {
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = GaugeDirection::Left,
  });
  Screen screen(11, 1);
  Render(screen, slider->Render());
  EXPECT_TRUE(slider->OnEvent(MousePressed(3, 0)));
  EXPECT_EQ(value, 70);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 0)));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 2)));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(slider->OnEvent(MousePressed(5, 2)));
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(5, 2)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(5, 2)));
}

TEST(SliderTest, Down) {
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = GaugeDirection::Down,
  });
  Screen screen(1, 11);
  Render(screen, slider->Render());
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 3)));
  EXPECT_EQ(value, 30);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 9)));
  EXPECT_EQ(value, 90);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 9)));
  EXPECT_EQ(value, 90);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(2, 5)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(2, 5)));
}

TEST(SliderTest, Up) {
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = GaugeDirection::Up,
  });
  Screen screen(1, 11);
  Render(screen, slider->Render());
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 3)));
  EXPECT_EQ(value, 70);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 9)));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 9)));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(2, 5)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(2, 5)));
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
