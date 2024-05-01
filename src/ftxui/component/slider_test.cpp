// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <array>                      // for array
#include <cstddef>                    // for size_t
#include <ftxui/component/mouse.hpp>  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <ftxui/dom/elements.hpp>   // for frame
#include <string>                   // for string, to_string

#include "ftxui/component/component.hpp"  // for Slider, Vertical, operator|=
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event, Event::ArrowDown
#include "ftxui/dom/node.hpp"                  // for Render
#include "ftxui/screen/screen.hpp"             // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, Test, EXPECT_EQ, EXPECT_TRUE, EXPECT_FALSE, TEST

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
      .direction = Direction::Right,
  });
  Screen screen(11, 1);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MousePressed(3, 0)));
  EXPECT_EQ(value, 50);
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
      .direction = Direction::Left,
  });
  Screen screen(11, 1);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MousePressed(3, 0)));
  EXPECT_EQ(value, 50);
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
      .direction = Direction::Down,
  });
  Screen screen(1, 11);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 3)));
  EXPECT_EQ(value, 50);
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
      .direction = Direction::Up,
  });
  Screen screen(1, 11);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 3)));
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 9)));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 9)));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(2, 5)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(2, 5)));
}

TEST(SliderTest, Focus) {
  static std::array<int, 10> values;
  auto container = Container::Vertical({});
  for (size_t i = 0; i < values.size(); ++i) {
    container->Add(Slider(std::to_string(i), &values[i]));
  }
  container |= frame;

  Screen screen(10, 3);

  Render(screen, container->Render());
  EXPECT_EQ(screen.at(0, 0), "0");  // Select 0
  EXPECT_EQ(screen.at(0, 1), "1");
  EXPECT_EQ(screen.at(0, 2), "2");

  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));
  Render(screen, container->Render());
  EXPECT_EQ(screen.at(0, 0), "0");
  EXPECT_EQ(screen.at(0, 1), "1");  // Select 1
  EXPECT_EQ(screen.at(0, 2), "2");

  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));
  Render(screen, container->Render());
  EXPECT_EQ(screen.at(0, 0), "1");
  EXPECT_EQ(screen.at(0, 1), "2");  // Select 2
  EXPECT_EQ(screen.at(0, 2), "3");

  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));  // Select 3
  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));  // Select 4
  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));  // Select 5
  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));  // Select 6

  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));
  Render(screen, container->Render());
  EXPECT_EQ(screen.at(0, 0), "6");
  EXPECT_EQ(screen.at(0, 1), "7");  // Select 7
  EXPECT_EQ(screen.at(0, 2), "8");

  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));
  Render(screen, container->Render());
  EXPECT_EQ(screen.at(0, 0), "7");
  EXPECT_EQ(screen.at(0, 1), "8");  // Select 8
  EXPECT_EQ(screen.at(0, 2), "9");

  EXPECT_TRUE(container->OnEvent(Event::ArrowDown));
  Render(screen, container->Render());
  EXPECT_EQ(screen.at(0, 0), "7");
  EXPECT_EQ(screen.at(0, 1), "8");
  EXPECT_EQ(screen.at(0, 2), "9");  // Select 9

  EXPECT_FALSE(container->OnEvent(Event::ArrowDown));
}

}  // namespace ftxui
// NOLINTEND
