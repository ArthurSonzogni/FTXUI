// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>  // for Test, EXPECT_EQ, Message, TestPartResult, TestInfo (ptr only), TEST
#include <chrono>                   // for operator""s, chrono_literals
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/animation.hpp"          // for Duration, Params
#include "ftxui/component/component.hpp"          // for Menu
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp, Event::Return
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen
#include "ftxui/util/ref.hpp"         // for Ref

// NOLINTBEGIN
namespace ftxui {

using namespace std::chrono_literals;

TEST(MenuTest, RemoveEntries) {
  int focused_entry = 0;
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto menu = Menu({
      .entries = &entries,
      .selected = &selected,
      .focused_entry = &focused_entry,
  });

  EXPECT_EQ(selected, 0);
  EXPECT_EQ(focused_entry, 0);

  menu->OnEvent(Event::ArrowDown);
  menu->OnEvent(Event::ArrowDown);
  menu->OnEvent(Event::Return);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  entries.resize(2);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  (void)menu->Render();

  EXPECT_EQ(selected, 1);
  EXPECT_EQ(focused_entry, 1);
}

TEST(MenuTest, DirectionDown) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  MenuOption option;
  auto menu = Menu(&entries, &selected, {.direction = Direction::Down});

  Screen screen(4, 3);
  Render(screen, menu->Render());
  EXPECT_EQ(screen.ToString(),
            "\x1B[1m\x1B[7m> 1 \x1B[22m\x1B[27m\r\n"
            "  2 \r\n"
            "  3 ");

  menu->OnEvent(Event::ArrowUp);
  EXPECT_EQ(selected, 0);
  menu->OnEvent(Event::ArrowDown);
  EXPECT_EQ(selected, 1);
  menu->OnEvent(Event::ArrowDown);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowDown);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 2);
}

TEST(MenuTest, DirectionsUp) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto menu = Menu(&entries, &selected, {.direction = Direction::Up});
  Screen screen(4, 3);
  Render(screen, menu->Render());
  EXPECT_EQ(screen.ToString(),
            "  3 \r\n"
            "  2 \r\n"
            "\x1B[1m\x1B[7m> 1 \x1B[22m\x1B[27m");
  menu->OnEvent(Event::ArrowDown);
  EXPECT_EQ(selected, 0);
  menu->OnEvent(Event::ArrowUp);
  EXPECT_EQ(selected, 1);
  menu->OnEvent(Event::ArrowUp);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowUp);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 2);
}

TEST(MenuTest, DirectionsRight) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto menu = Menu(&entries, &selected, {.direction = Direction::Right});
  Screen screen(10, 1);
  Render(screen, menu->Render());
  EXPECT_EQ(screen.ToString(),
            "\x1B[1m\x1B[7m> 1\x1B[22m\x1B[27m"
            "  2"
            "  3 ");
  menu->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 0);
  menu->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 1);
  menu->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowUp);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowDown);
  EXPECT_EQ(selected, 2);
}

TEST(MenuTest, DirectionsLeft) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto menu = Menu(&entries, &selected, {.direction = Direction::Left});
  Screen screen(10, 1);
  Render(screen, menu->Render());
  EXPECT_EQ(screen.ToString(),
            "  3"
            "  2"
            "\x1B[1m\x1B[7m> 1\x1B[22m\x1B[27m ");
  menu->OnEvent(Event::ArrowRight);
  EXPECT_EQ(selected, 0);
  menu->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 1);
  menu->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowUp);
  EXPECT_EQ(selected, 2);
  menu->OnEvent(Event::ArrowDown);
  EXPECT_EQ(selected, 2);
}

TEST(MenuTest, AnimationsHorizontal) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto menu = Menu(&entries, &selected, MenuOption::HorizontalAnimated());
  {
    Screen screen(4, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[1m\x1B[7m1\x1B[22m\x1B[27m \x1B[2m2\x1B[22m "
        "\r\n\x1B[97m\x1B[49m\xE2\x94\x80\x1B[90m\x1B["
        "49m\xE2\x95\xB6\xE2\x94\x80\xE2\x94\x80\x1B[39m\x1B[49m\r\n    ");
  }
  selected = 1;
  {
    Screen screen(4, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[7m1\x1B[27m \x1B[1m2\x1B[22m "
        "\r\n\x1B[97m\x1B[49m\xE2\x94\x80\x1B[90m\x1B["
        "49m\xE2\x95\xB6\xE2\x94\x80\xE2\x94\x80\x1B[39m\x1B[49m\r\n    ");
  }
  animation::Params params(2s);
  menu->OnAnimation(params);
  {
    Screen screen(4, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[7m1\x1B[27m \x1B[1m2\x1B[22m "
        "\r\n\x1B[90m\x1B[49m\xE2\x94\x80\xE2\x95\xB4\x1B[97m\x1B["
        "49m\xE2\x94\x80\x1B[90m\x1B[49m\xE2\x95\xB6\x1B[39m\x1B[49m\r\n    ");
  }
}

TEST(MenuTest, AnimationsVertical) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto menu = Menu(&entries, &selected, MenuOption::VerticalAnimated());
  {
    Screen screen(10, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[90m\x1B[49m\xE2\x94\x82\x1B[1m\x1B[7m\x1B[39m\x1B[49m1\x1B["
        "22m\x1B[27m        "
        "\r\n\x1B[97m\x1B[49m\xE2\x95\xB7\x1B[2m\x1B[39m\x1B[49m2\x1B[22m      "
        "  \r\n\x1B[97m\x1B[49m\xE2\x94\x82\x1B[2m\x1B[39m\x1B[49m3\x1B[22m    "
        "    ");
  }
  selected = 1;
  {
    Screen screen(10, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[90m\x1B[49m\xE2\x94\x82\x1B[7m\x1B[39m\x1B[49m1\x1B[27m        "
        "\r\n\x1B[97m\x1B[49m\xE2\x95\xB7\x1B[1m\x1B[39m\x1B[49m2\x1B[22m      "
        "  \r\n\x1B[97m\x1B[49m\xE2\x94\x82\x1B[2m\x1B[39m\x1B[49m3\x1B[22m    "
        "    ");
  }
  animation::Params params(2s);
  menu->OnAnimation(params);
  {
    Screen screen(10, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(
        screen.ToString(),
        "\x1B[97m\x1B[49m\xE2\x95\xB5\x1B[7m\x1B[39m\x1B[49m1\x1B[27m        "
        "\r\n\x1B[90m\x1B[49m\xE2\x94\x82\x1B[1m\x1B[39m\x1B[49m2\x1B[22m      "
        "  \r\n\x1B[97m\x1B[49m\xE2\x95\xB7\x1B[2m\x1B[39m\x1B[49m3\x1B[22m    "
        "    ");
  }
}

}  // namespace ftxui
// NOLINTEND
