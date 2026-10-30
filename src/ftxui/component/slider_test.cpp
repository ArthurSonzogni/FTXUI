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
  int updated = 0;
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Right,
      .on_change = [&]() { updated++; },
  });
  Screen screen(11, 1);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 0);
  EXPECT_TRUE(slider->OnEvent(MousePressed(3, 0)));
  EXPECT_EQ(value, 30);
  EXPECT_EQ(updated, 1);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 0)));
  EXPECT_EQ(value, 90);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 2)));
  EXPECT_EQ(value, 90);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(5, 2)));
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 3);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(5, 2)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(5, 2)));
  EXPECT_EQ(value, 50);
}

TEST(SliderTest, Left) {
  int updated = 0;
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Left,
      .on_change = [&]() { updated++; },
  });
  Screen screen(11, 1);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 0);
  EXPECT_TRUE(slider->OnEvent(MousePressed(3, 0)));
  EXPECT_EQ(value, 70);
  EXPECT_EQ(updated, 1);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 0)));
  EXPECT_EQ(value, 10);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(9, 2)));
  EXPECT_EQ(value, 10);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(5, 2)));
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 3);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(5, 2)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(5, 2)));
  EXPECT_EQ(value, 50);
}

TEST(SliderTest, Down) {
  int updated = 0;
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Down,
      .on_change = [&]() { updated++; },
  });
  Screen screen(1, 11);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 0);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 3)));
  EXPECT_EQ(value, 30);
  EXPECT_EQ(updated, 1);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 9)));
  EXPECT_EQ(value, 90);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 9)));
  EXPECT_EQ(value, 90);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 3);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(2, 5)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 3);
}

TEST(SliderTest, Up) {
  int updated = 0;
  int value = 50;
  auto slider = Slider<int>({
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Up,
      .on_change = [&]() { updated++; },
  });
  Screen screen(1, 11);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 0);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 3)));
  EXPECT_EQ(value, 70);
  EXPECT_EQ(updated, 1);
  EXPECT_TRUE(slider->OnEvent(MousePressed(0, 9)));
  EXPECT_EQ(value, 10);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 9)));
  EXPECT_EQ(value, 10);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 3);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(2, 5)));
  EXPECT_FALSE(slider->OnEvent(MousePressed(2, 5)));
  EXPECT_EQ(value, 50);
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

TEST(SliderTest, LabeledSliderIsOneLineHigh) {
  int value = 50;
  auto button_left = Button("button 1", [] {});
  auto slider = Slider("slider", &value, 0, 100, 1);
  auto button_right = Button("button 2", [] {});
  auto container =
      Container::Horizontal({button_left, slider, button_right});

  Screen screen(40, 3);
  Render(screen, container->Render());

  EXPECT_EQ(screen.at(16, 0), "");
  EXPECT_EQ(screen.at(16, 1), "[");
  EXPECT_EQ(screen.at(29, 1), "]");
  EXPECT_EQ(screen.at(16, 2), "");
}

// SliderOption::label: the label must be rendered.
// Layout for Screen(15, 1): "ab" (x 0-1), "[" (x 2), gauge (x 3-13), "]" (x 14)
TEST(SliderTest, LabeledOptionsRendersLabel) {
  int value = 50;
  auto slider = Slider<int>({
      .label = "ab",
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Right,
  });
  Screen screen(15, 1);
  Render(screen, slider->Render());
  EXPECT_EQ(screen.at(0, 0), "a");
  EXPECT_EQ(screen.at(1, 0), "b");
  EXPECT_EQ(screen.at(2, 0), "[");
  EXPECT_EQ(screen.at(14, 0), "]");
}

// Mouse events must reach the slider when using label.
TEST(SliderTest, LabeledOptionsMouse) {
  int updated = 0;
  int value = 50;
  auto slider = Slider<int>({
      .label = "ab",
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Right,
      .on_change = [&]() { updated++; },
  });
  Screen screen(15, 1);
  Render(screen, slider->Render());
  EXPECT_EQ(value, 50);
  EXPECT_EQ(updated, 0);

  // The gauge spans x = 3..13, so x = 6 -> 30 and x = 11 -> 80.
  EXPECT_TRUE(slider->OnEvent(MousePressed(6, 0)));
  EXPECT_EQ(value, 30);
  EXPECT_EQ(updated, 1);
  EXPECT_TRUE(slider->OnEvent(MousePressed(11, 0)));
  EXPECT_EQ(value, 80);
  EXPECT_EQ(updated, 2);
  EXPECT_TRUE(slider->OnEvent(MouseReleased(11, 0)));
}

// Keyboard events must work when using label.
TEST(SliderTest, LabeledOptionsKeyboard) {
  int updated = 0;
  int value = 50;
  auto slider = Slider<int>({
      .label = "label",
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Right,
      .on_change = [&]() { updated++; },
  });
  EXPECT_TRUE(slider->OnEvent(Event::ArrowRight));
  EXPECT_EQ(value, 60);
  EXPECT_EQ(updated, 1);
  EXPECT_TRUE(slider->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(slider->OnEvent(Event::ArrowLeft));
  EXPECT_EQ(value, 40);
  EXPECT_EQ(updated, 3);

  // Clamped at the bounds: no change, so the event is not consumed.
  value = 100;
  EXPECT_FALSE(slider->OnEvent(Event::ArrowRight));
  EXPECT_EQ(value, 100);
  EXPECT_EQ(updated, 3);
}

// Float and double types must work with label.
TEST(SliderTest, LabeledOptionsFloatAndDouble) {
  float f = 0.5f;
  auto slider_f = Slider<float>({
      .label = "f",
      .value = &f,
      .min = 0.f,
      .max = 1.f,
      .increment = 0.25f,
      .direction = Direction::Right,
  });
  EXPECT_TRUE(slider_f->OnEvent(Event::ArrowRight));
  EXPECT_FLOAT_EQ(f, 0.75f);

  double d = 0.5;
  auto slider_d = Slider<double>({
      .label = "d",
      .value = &d,
      .min = 0.0,
      .max = 1.0,
      .increment = 0.25,
      .direction = Direction::Right,
  });
  EXPECT_TRUE(slider_d->OnEvent(Event::ArrowLeft));
  EXPECT_DOUBLE_EQ(d, 0.25);
}

// Guards against dropping `vcenter`.
TEST(SliderTest, LabeledOptionsSliderIsOneLineHigh) {
  int value = 50;
  auto button_left = Button("button 1", [] {});
  auto slider = Slider<int>({
      .label = "slider",
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 1,
      .direction = Direction::Right,
  });
  auto button_right = Button("button 2", [] {});
  auto container = Container::Horizontal({button_left, slider, button_right});

  Screen screen(40, 3);
  Render(screen, container->Render());

  EXPECT_EQ(screen.at(16, 0), "");
  EXPECT_EQ(screen.at(16, 1), "[");
  EXPECT_EQ(screen.at(29, 1), "]");
  EXPECT_EQ(screen.at(16, 2), "");
}

// Labeled vertical slider renders without brackets/horizontal distortion.
TEST(SliderTest, LabeledOptionsVertical) {
  int value = 50;
  auto slider = Slider<int>({
      .label = "v",
      .value = &value,
      .min = 0,
      .max = 100,
      .increment = 10,
      .direction = Direction::Down,
  });
  Screen screen(1, 12);
  Render(screen, slider->Render());
  EXPECT_EQ(screen.at(0, 0), "v");
}

// Custom colors are applied to active and inactive states.
TEST(SliderTest, LabeledOptionsColors) {
  int value1 = 50;
  int value2 = 50;
  auto slider1 = Slider<int>({
      .label = "lbl1",
      .value = &value1,
      .color_active = Color::Red,
      .color_inactive = Color::Blue,
  });
  auto slider2 = Slider<int>({
      .label = "lbl2",
      .value = &value2,
      .color_active = Color::Red,
      .color_inactive = Color::Blue,
  });
  auto container = Container::Vertical({slider1, slider2});
  Screen screen(15, 2);
  Render(screen, container->Render());
  EXPECT_EQ(screen.PixelAt(0, 0).foreground_color, Color(Color::Red));
  EXPECT_EQ(screen.PixelAt(0, 1).foreground_color, Color(Color::Blue));
}

}  // namespace ftxui
// NOLINTEND
