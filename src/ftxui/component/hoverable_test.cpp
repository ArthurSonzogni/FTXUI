// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/dom/elements.hpp>  // for Element, text
#include <string>                  // for string

#include "ftxui/component/component.hpp"  // for Hoverable, Horizontal, operator|=, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_FALSE, EXPECT_EQ, Test, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

namespace {
Event HoverEvent(int x, int y) {
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

Component BasicComponent() {
  return Renderer([] { return text("[ ]"); });
}

TEST(HoverableTest, BasicBool) {
  bool hover_1 = false;
  bool hover_2 = false;
  auto c1 = Hoverable(BasicComponent(), &hover_1);
  auto c2 = Hoverable(BasicComponent(), &hover_2);
  auto layout = Container::Horizontal({c1, c2});
  auto screen = Screen(8, 2);
  Render(screen, layout->Render());
  EXPECT_FALSE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(1, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(2, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(3, 0)));
  EXPECT_FALSE(hover_1);
  EXPECT_TRUE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);
}

TEST(HoverableTest, BasicCallback) {
  int on_enter_1 = 0;
  int on_enter_2 = 0;
  int on_leave_1 = 0;
  int on_leave_2 = 0;
  auto c1 =
      Hoverable(BasicComponent(), [&] { on_enter_1++; }, [&] { on_leave_1++; });
  auto c2 =
      Hoverable(BasicComponent(), [&] { on_enter_2++; }, [&] { on_leave_2++; });
  auto layout = Container::Horizontal({c1, c2});
  auto screen = Screen(8, 2);
  Render(screen, layout->Render());
  EXPECT_EQ(on_enter_1, 0);
  EXPECT_EQ(on_enter_2, 0);
  EXPECT_EQ(on_leave_1, 0);
  EXPECT_EQ(on_leave_2, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_EQ(on_enter_1, 1);
  EXPECT_EQ(on_enter_2, 0);
  EXPECT_EQ(on_leave_1, 0);
  EXPECT_EQ(on_leave_2, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(1, 0)));
  EXPECT_EQ(on_enter_1, 1);
  EXPECT_EQ(on_enter_2, 0);
  EXPECT_EQ(on_leave_1, 0);
  EXPECT_EQ(on_leave_2, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(2, 0)));
  EXPECT_EQ(on_enter_1, 1);
  EXPECT_EQ(on_enter_2, 0);
  EXPECT_EQ(on_leave_1, 0);
  EXPECT_EQ(on_leave_2, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(3, 0)));
  EXPECT_EQ(on_enter_1, 1);
  EXPECT_EQ(on_enter_2, 1);
  EXPECT_EQ(on_leave_1, 1);
  EXPECT_EQ(on_leave_2, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_EQ(on_enter_1, 2);
  EXPECT_EQ(on_enter_2, 1);
  EXPECT_EQ(on_leave_1, 1);
  EXPECT_EQ(on_leave_2, 1);
}

TEST(HoverableTest, BasicBoolCallback) {
  bool hover_1 = false;
  bool hover_2 = false;
  auto c1 = Hoverable(BasicComponent(), [&](bool hover) { hover_1 = hover; });
  auto c2 = Hoverable(BasicComponent(), [&](bool hover) { hover_2 = hover; });
  auto layout = Container::Horizontal({c1, c2});
  auto screen = Screen(8, 2);
  Render(screen, layout->Render());
  EXPECT_FALSE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(1, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(2, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(3, 0)));
  EXPECT_FALSE(hover_1);
  EXPECT_TRUE(hover_2);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_FALSE(hover_2);
}

TEST(HoverableTest, Coverage) {
  bool hover_1 = false;
  bool hover_2 = false;
  int on_enter = 0;
  int on_leave = 0;
  auto c1 = BasicComponent();
  c1 |= Hoverable(&hover_1);
  c1 |= Hoverable([&](bool hover) { hover_2 = hover; });
  c1 |= Hoverable([&] { on_enter++; }, [&] { on_leave++; });
  auto c2 = BasicComponent();
  auto layout = Container::Horizontal({c1, c2});

  auto screen = Screen(8, 2);
  Render(screen, layout->Render());
  EXPECT_FALSE(hover_1);
  EXPECT_FALSE(hover_2);
  EXPECT_EQ(on_enter, 0);
  EXPECT_EQ(on_leave, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_TRUE(hover_2);
  EXPECT_EQ(on_enter, 1);
  EXPECT_EQ(on_leave, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(1, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_TRUE(hover_2);
  EXPECT_EQ(on_enter, 1);
  EXPECT_EQ(on_leave, 0);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(3, 0)));
  EXPECT_FALSE(hover_1);
  EXPECT_FALSE(hover_2);
  EXPECT_EQ(on_enter, 1);
  EXPECT_EQ(on_leave, 1);

  EXPECT_FALSE(layout->OnEvent(HoverEvent(0, 0)));
  EXPECT_TRUE(hover_1);
  EXPECT_TRUE(hover_2);
  EXPECT_EQ(on_enter, 2);
  EXPECT_EQ(on_leave, 1);
}

}  // namespace
}  // namespace ftxui
// NOLINTEND
