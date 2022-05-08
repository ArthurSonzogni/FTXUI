#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <chrono>                   // for operator""s, chrono_literals
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string

#include "ftxui/component/animation.hpp"          // for Duration, Params
#include "ftxui/component/component.hpp"          // for Button, Horizontal
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowLeft, Event::ArrowRight
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen
#include "ftxui/screen/terminal.hpp"  // for SetColorSupport, Color, TrueColor
#include "gtest/gtest_pred_impl.h"  // for AssertionResult, EXPECT_EQ, Test, EXPECT_FALSE, EXPECT_TRUE, TEST

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

using namespace std::chrono_literals;

TEST(ButtonTest, Basic) {
  int press_count = 0;
  std::string last_press = "";
  auto btn1 = Button("btn1", [&] {
    press_count++;
    last_press = "btn1";
  });
  auto btn2 = Button("btn2", [&] {
    press_count++;
    last_press = "btn2";
  });

  int selected = 0;
  auto container = Container::Horizontal(
      {
          btn1,
          btn2,
      },
      &selected);

  (void)container->Render();

  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(btn1->Focused());
  EXPECT_FALSE(btn2->Focused());

  container->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(btn1->Focused());
  EXPECT_FALSE(btn2->Focused());

  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  EXPECT_FALSE(btn1->Focused());
  EXPECT_TRUE(btn2->Focused());

  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  EXPECT_FALSE(btn1->Focused());
  EXPECT_TRUE(btn2->Focused());

  EXPECT_EQ(press_count, 0);

  container->OnEvent(Event::Return);
  EXPECT_EQ(press_count, 1);
  EXPECT_EQ(last_press, "btn2");

  container->OnEvent(Event::Return);
  EXPECT_EQ(press_count, 2);
  EXPECT_EQ(last_press, "btn2");

  container->OnEvent(Event::ArrowLeft);
  container->OnEvent(Event::Return);
  EXPECT_EQ(press_count, 3);
  EXPECT_EQ(last_press, "btn1");

  (void)container->Render();
}

TEST(ButtonTest, Animation) {
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  int press_count = 0;
  std::string last_press = "";
  auto option = ButtonOption::Animated();
  auto btn1 = Button(
      "btn1",
      [&] {
        press_count++;
        last_press = "btn1";
      },
      option);
  auto btn2 = Button(
      "btn2",
      [&] {
        press_count++;
        last_press = "btn2";
      },
      option);

  int selected = 0;
  auto container = Container::Horizontal(
      {
          btn1,
          btn2,
      },
      &selected);

  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(screen.ToString(),
              "\x1B[1m\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m      \x1B[22m     "
              " \x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;2;192;192;192m\x1B[48;2;0;0;"
              "0m btn1 \x1B[22m btn2 "
              "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;2;192;192;192m\x1B[48;2;0;0;"
              "0m      \x1B[22m      \x1B[39m\x1B[49m");
  }
  selected = 1;
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(screen.ToString(),
              "\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m      \x1B[1m      "
              "\x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;2;192;192;192m\x1B[48;2;0;0;"
              "0m btn1 \x1B[1m btn2 "
              "\x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;2;192;192;192m\x1B[48;2;0;0;"
              "0m      \x1B[1m      \x1B[22m\x1B[39m\x1B[49m");
  }
  animation::Params params(2s);
  container->OnAnimation(params);
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m      "
        "\x1B[1m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m      "
        "\x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m "
        "btn1 \x1B[1m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m btn2 "
        "\x1B[22m\x1B[39m\x1B[49m\r\n\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m    "
        "  \x1B[1m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m      "
        "\x1B[22m\x1B[39m\x1B[49m");
  }
  EXPECT_EQ(selected, 1);
  container->OnEvent(MousePressed(3, 1));
  EXPECT_EQ(selected, 0);
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[1m\x1B[38;2;223;223;223m\x1B[48;2;64;64;64m      "
        "\x1B[22m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m      "
        "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;2;223;223;223m\x1B[48;2;64;64;64m "
        "btn1 \x1B[22m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m btn2 "
        "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;2;223;223;223m\x1B[48;2;64;64;64m  "
        "    \x1B[22m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m      "
        "\x1B[39m\x1B[49m");
  }
  container->OnAnimation(params);
  {
    Screen screen(12, 3);
    Render(screen, container->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[1m\x1B[38;2;255;255;255m\x1B[48;2;128;128;128m      "
        "\x1B[22m\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m      "
        "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;2;255;255;255m\x1B[48;2;128;128;"
        "128m btn1 \x1B[22m\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m btn2 "
        "\x1B[39m\x1B[49m\r\n\x1B[1m\x1B[38;2;255;255;255m\x1B[48;2;128;128;"
        "128m      \x1B[22m\x1B[38;2;192;192;192m\x1B[48;2;0;0;0m      "
        "\x1B[39m\x1B[49m");
  }
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
