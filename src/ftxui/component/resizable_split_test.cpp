#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Radiobox
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for RadioboxOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown, Event::ArrowUp, Event::Tab, Event::TabReverse
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"    // for EXPECT_EQ, Test, TEST

namespace ftxui {

namespace {
Component BasicComponent() {
  return Renderer([] { return text(""); });
}

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

TEST(ResizableSplit, BasicLeft) {
  int position = 3;
  auto component =
      ResizableSplitLeft(BasicComponent(), BasicComponent(), &position);
  auto screen = Screen(20, 20);
  Render(screen, component->Render());
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(3, 1)));
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(10, 1)));
  EXPECT_EQ(position, 10);
  EXPECT_TRUE(component->OnEvent(MouseReleased(10, 1)));
  EXPECT_EQ(position, 10);
}

TEST(ResizableSplit, BasicRight) {
  int position = 3;
  auto component =
      ResizableSplitRight(BasicComponent(), BasicComponent(), &position);
  auto screen = Screen(20, 20);
  Render(screen, component->Render());
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(16, 1)));
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(10, 1)));
  EXPECT_EQ(position, 9);
  EXPECT_TRUE(component->OnEvent(MouseReleased(10, 1)));
  EXPECT_EQ(position, 9);
}

TEST(ResizableSplit, BasicTop) {
  int position = 3;
  auto component =
      ResizableSplitTop(BasicComponent(), BasicComponent(), &position);
  auto screen = Screen(20, 20);
  Render(screen, component->Render());
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(1, 3)));
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(1, 10)));
  EXPECT_EQ(position, 10);
  EXPECT_TRUE(component->OnEvent(MouseReleased(1, 10)));
  EXPECT_EQ(position, 10);
}

TEST(ResizableSplit, BasicBottom) {
  int position = 3;
  auto component =
      ResizableSplitBottom(BasicComponent(), BasicComponent(), &position);
  auto screen = Screen(20, 20);
  Render(screen, component->Render());
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(1, 16)));
  EXPECT_EQ(position, 3);
  EXPECT_TRUE(component->OnEvent(MousePressed(1, 10)));
  EXPECT_EQ(position, 9);
  EXPECT_TRUE(component->OnEvent(MouseReleased(1, 10)));
  EXPECT_EQ(position, 9);
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
