// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <csignal>  // for raise, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM

#include "ftxui/component/component.hpp"  // for Input, Renderer, Vertical
#include "ftxui/component/event.hpp"      // for Event
#include "ftxui/component/loop.hpp"       // for Loop
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"   // for text
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

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
  return Event::Mouse("", mouse);
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
  return Event::Mouse("", mouse);
}

Event MouseMove(int x, int y) {
  Mouse mouse;
  mouse.button = Mouse::Left;
  mouse.motion = Mouse::Moved;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;
  mouse.x = x;
  mouse.y = y;
  return Event::Mouse("", mouse);
}

}  // namespace

TEST(SelectionTest, DefaultSelection) {
  auto component = Renderer([&] { return text("Lorem ipsum dolor"); });
  auto screen = ScreenInteractive::FixedSize(20, 1);
  EXPECT_EQ(screen.GetSelection(), "");
  Loop loop(&screen, component);
  screen.PostEvent(MousePressed(3, 1));
  screen.PostEvent(MouseReleased(10, 1));
  loop.RunOnce();

  EXPECT_EQ(screen.GetSelection(), "rem ipsu");
}

TEST(SelectionTest, SelectionChange) {
  int selectionChangeCounter = 0;
  auto component = Renderer([&] { return text("Lorem ipsum dolor"); });
  auto screen = ScreenInteractive::FixedSize(20, 1);
  screen.SelectionChange([&] { selectionChangeCounter++; });

  Loop loop(&screen, component);
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 0);

  screen.PostEvent(MousePressed(3, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 0);

  screen.PostEvent(MouseMove(5, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 1);

  screen.PostEvent(MouseMove(7, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 2);

  screen.PostEvent(MouseReleased(10, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 3);

  screen.PostEvent(MouseMove(10, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 3);

  EXPECT_EQ(screen.GetSelection(), "rem ipsu");
}

// Check that submitting multiple mouse events quickly doesn't trigger multiple
// selection change events.
TEST(SelectionTest, SelectionOnChangeSquashedEvents) {
  int selectionChangeCounter = 0;
  auto component = Renderer([&] { return text("Lorem ipsum dolor"); });
  auto screen = ScreenInteractive::FixedSize(20, 1);
  screen.SelectionChange([&] { selectionChangeCounter++; });

  Loop loop(&screen, component);
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 0);

  screen.PostEvent(MousePressed(3, 1));
  screen.PostEvent(MouseMove(5, 1));
  screen.PostEvent(MouseMove(7, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 1);

  screen.PostEvent(MouseReleased(10, 1));
  screen.PostEvent(MouseMove(10, 1));
  loop.RunOnce();
  EXPECT_EQ(selectionChangeCounter, 2);

  EXPECT_EQ(screen.GetSelection(), "rem ipsu");
}

TEST(SelectionTest, StyleSelection) {
  auto element = hbox({
      text("Lorem "),
      text("ipsum") | selectionColor(Color::Red),
      text(" dolor"),
  });

  auto screen = ScreenInteractive::FixedSize(20, 1);
  Selection selection(2, 0, 9, 0);

  Render(screen, element.get(), selection);
  for (int i = 0; i < 20; i++) {
    if (i >= 2 && i <= 9) {
      EXPECT_EQ(screen.PixelAt(i, 0).inverted, true);
    } else {
      EXPECT_EQ(screen.PixelAt(i, 0).inverted, false);
    }

    if (i >= 6 && i <= 9) {
      EXPECT_EQ(screen.PixelAt(i, 0).foreground_color, Color::Red);
    } else {
      EXPECT_EQ(screen.PixelAt(i, 0).foreground_color, Color::Default);
    }
  }
}

TEST(SelectionTest, VBoxSelection) {
  auto element = vbox({
      text("Lorem ipsum dolor"),
      text("Ut enim ad minim"),
  });

  auto screen = ScreenInteractive::FixedSize(20, 2);

  Selection selection(2, 0, 2, 1);
  Render(screen, element.get(), selection);

  EXPECT_EQ(selection.GetParts(), "rem ipsum dolor\nUt ");
  EXPECT_EQ(screen.ToString(),
            "Lo\x1B[7mrem ipsum dolor\x1B[27m   \r\n"
            "\x1B[7mUt \x1B[27menim ad minim    ");
}

TEST(SelectionTest, VBoxSaturatedSelection) {
  auto element = vbox({
      text("Lorem ipsum dolor"),
      text("Ut enim ad minim"),
      text("Duis aute irure"),
  });

  auto screen = ScreenInteractive::FixedSize(20, 3);
  Selection selection(2, 0, 2, 2);
  Render(screen, element.get(), selection);
  EXPECT_EQ(selection.GetParts(), "rem ipsum dolor\nUt enim ad minim\nDui");

  EXPECT_EQ(screen.ToString(),
            "Lo\x1B[7mrem ipsum dolor\x1B[27m   \r\n"
            "\x1B[7mUt enim ad minim\x1B[27m    \r\n"
            "\x1B[7mDui\x1B[27ms aute irure     ");
}

TEST(SelectionTest, HBoxSelection) {
  auto element = hbox({
      text("Lorem ipsum dolor"),
      text("Ut enim ad minim"),
  });

  auto screen = ScreenInteractive::FixedSize(40, 1);
  Selection selection(2, 0, 20, 0);
  Render(screen, element.get(), selection);
  EXPECT_EQ(selection.GetParts(), "rem ipsum dolorUt e");
  EXPECT_EQ(screen.ToString(),
            "Lo\x1B[7mrem ipsum dolorUt e\x1B[27mnim ad minim       ");
}

TEST(SelectionTest, HBoxSaturatedSelection) {
  auto element = hbox({
      text("Lorem ipsum dolor"),
      text("Ut enim ad minim"),
      text("Duis aute irure"),
  });

  auto screen = ScreenInteractive::FixedSize(60, 1);

  Selection selection(2, 0, 35, 0);
  Render(screen, element.get(), selection);
  EXPECT_EQ(selection.GetParts(), "rem ipsum dolorUt enim ad minimDui");
  EXPECT_EQ(screen.ToString(),
            "Lo\x1B[7mrem ipsum dolorUt enim ad minimDui\x1B[27ms aute irure   "
            "         ");
}

}  // namespace ftxui
// NOLINTEND
