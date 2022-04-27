#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Menu
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::Return
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

namespace ftxui {

TEST(MenuTest, RemoveEntries) {
  int focused_entry = 0;
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  MenuOption option;
  option.focused_entry = &focused_entry;
  auto menu = Menu(&entries, &selected, option);

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

TEST(MenuTest, Directions) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  MenuOption option;
  auto menu = Menu(&entries, &selected, &option);

  {
    option.direction = MenuOption::Down;
    Screen screen(4, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(screen.ToString(),
              "\x1B[1m\x1B[7m> 1 \x1B[22m\x1B[27m\r\n"
              "  2 \r\n"
              "  3 ");
  }

  {
    option.direction = MenuOption::Up;
    Screen screen(4, 3);
    Render(screen, menu->Render());
    EXPECT_EQ(screen.ToString(),
              "  3 \r\n"
              "  2 \r\n"
              "\x1B[1m\x1B[7m> 1 \x1B[22m\x1B[27m");
  }

  {
    option.direction = MenuOption::Right;
    Screen screen(10, 1);
    Render(screen, menu->Render());
    EXPECT_EQ(screen.ToString(),
              "\x1B[1m\x1B[7m> 1\x1B[22m\x1B[27m"
              "  2"
              "  3 ");
  }

  {
    option.direction = MenuOption::Left;
    Screen screen(10, 1);
    Render(screen, menu->Render());
    EXPECT_EQ(screen.ToString(),
              "  3"
              "  2"
              "\x1B[1m\x1B[7m> 1\x1B[22m\x1B[27m ");
  }
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
