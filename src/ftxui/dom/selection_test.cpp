// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <csignal>  // for raise, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM

#include "ftxui/dom/elements.hpp"  // for text
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/loop.hpp"       // for Loop


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


TEST(SelectionTest, DefaultSelection) {

  auto component = Renderer([&] {
      return text("Lorem ipsum dolor");
  });

  auto screen = ScreenInteractive::FixedSize(20, 1);

  Loop loop(&screen, component);

  loop.RunOnce();
  screen.PostEvent(MousePressed(3, 1));
  loop.RunOnce();
  screen.PostEvent(MouseReleased(10, 1));
  loop.RunOnce();

  EXPECT_STREQ(screen.GetSelectedContent(component).c_str(), "rem ipsu");
}

TEST(SelectionTest, CallbackSelection) {

  int selectionChangeCounter = 0;

  auto component = Renderer([&] {
      return text("Lorem ipsum dolor");
  });

  auto screen = ScreenInteractive::FixedSize(20, 1);

  screen.setSelectionOptions({
    .transform = [](Pixel& pixel) {
      pixel.underlined_double = true;
    },
    .on_change = [&] {
      selectionChangeCounter++;
    }
  });

  Loop loop(&screen, component);

  loop.RunOnce();
  screen.PostEvent(MousePressed(3, 1));
  loop.RunOnce();
  screen.PostEvent(MouseReleased(10, 1));
  loop.RunOnce();

  EXPECT_EQ(selectionChangeCounter, 2);

  EXPECT_STREQ(screen.GetSelectedContent(component).c_str(), "rem ipsu");
}

TEST(SelectionTest, StyleSelection) {

  int selectionChangeCounter = 0;

  auto component = Renderer([&] {
      return text("Lorem ipsum dolor");
  });

  auto screen = ScreenInteractive::FixedSize(20, 1);

  Selection selection(2, 0, 9, 0, {
    .transform = [](Pixel& pixel) {
      pixel.underlined_double = true;
    },
    .on_change = [&] {
    }
  });

  Render(screen, component->Render().get(), selection);

  EXPECT_EQ(screen.ToString(), "Lo\x1B[21mrem ipsu\x1B[24mm dolor   ");
}



}  // namespace ftxui
// NOLINTEND
