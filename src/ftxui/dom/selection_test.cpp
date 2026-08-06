// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <csignal>  // for raise, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM

#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"  // for Input, Renderer, Vertical
#include "ftxui/component/event.hpp"      // for Event
#include "ftxui/component/loop.hpp"       // for Loop
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
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
  auto screen = App::FixedSize(20, 1);
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
  auto screen = App::FixedSize(20, 1);
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
  auto screen = App::FixedSize(20, 1);
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

  auto screen = App::FixedSize(20, 1);
  Selection selection(2, 0, 9, 0);

  Render(screen, element.get(), selection);
  for (int i = 0; i < 20; i++) {
    if (i >= 2 && i <= 9) {
      EXPECT_EQ(screen.CellAt(i, 0).inverted, true);
    } else {
      EXPECT_EQ(screen.CellAt(i, 0).inverted, false);
    }

    if (i >= 6 && i <= 9) {
      EXPECT_EQ(screen.CellAt(i, 0).foreground_color, Color::Red);
    } else {
      EXPECT_EQ(screen.CellAt(i, 0).foreground_color, Color::Default);
    }
  }
}

// The selection style must not persist on an element rendered again after the
// selection was cleared. See #1309.
TEST(SelectionTest, StyleClearedWhenSelectionCleared) {
  auto element = text("Lorem ipsum dolor");

  {
    auto screen = App::FixedSize(20, 1);
    Selection selection(2, 0, 9, 0);
    Render(screen, element.get(), selection);
    EXPECT_EQ(screen.CellAt(2, 0).inverted, true);
  }

  {
    auto screen = App::FixedSize(20, 1);
    Render(screen, element);
    for (int i = 0; i < 20; i++) {
      EXPECT_EQ(screen.CellAt(i, 0).inverted, false);
    }
  }
}

// A selection over a subset of the lines of a multi-line text must style and
// extract exactly the selected rows.
TEST(SelectionTest, MultiLineTextSelection) {
  auto element = text("abc\ndef\nghi");
  auto screen = App::FixedSize(3, 3);
  Selection selection(1, 1, 2, 2);
  Render(screen, element.get(), selection);

  EXPECT_EQ(selection.GetParts(), "ef\nghi");
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      const bool selected = (y == 1 && x >= 1) || y == 2;
      EXPECT_EQ(screen.CellAt(x, y).inverted, selected)
          << "at (" << x << "," << y << ")";
    }
  }
}

TEST(SelectionTest, VBoxSelection) {
  auto element = vbox({
      text("Lorem ipsum dolor"),
      text("Ut enim ad minim"),
  });

  auto screen = App::FixedSize(20, 2);

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

  auto screen = App::FixedSize(20, 3);
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

  auto screen = App::FixedSize(40, 1);
  Selection selection(2, 0, 20, 0);
  Render(screen, element.get(), selection);
  EXPECT_EQ(selection.GetParts(), "rem ipsum dolorUt e");
  EXPECT_EQ(screen.ToString(),
            "Lo\x1B[7mrem ipsum dolorUt e\x1B[27mnim ad minim       ");
}

// The next two tests drive a selection through `App`/`Loop` (press, move,
// release, each in its own frame), then re-render the same document with a
// plain `Selection` built from `screen.GetSelection()`'s row/column extent
// to check exactly what got highlighted. `screen.CellAt()` can't be used
// directly after `loop.RunOnce()`: `App::Internal::Draw()` clears its cell
// buffer at the end of every frame (to build the next diff), so by the time
// a test can inspect it, it's already back to defaults.

// A selection built up over several separate frames (press, then one or
// more moves, then release each processed in their own `loop.RunOnce()`)
// must render the same thing as if it had been submitted all at once: the
// first row from the start column to the end of the line, the last row from
// the start of the line to the end column, and every row in between fully
// selected (this is regular flow/text selection, not a rectangular block
// selection -- see the pre-existing VBoxSelection/HBoxSelection tests).
TEST(SelectionTest, SelectionAcrossMultipleFrames) {
  // Each row is "0123456": column `x` holds the digit `x`.
  const std::string content =
      "0123456\n0123456\n0123456\n0123456\n0123456\n0123456\n0123456";
  auto component = Renderer([&] { return text(content); });
  auto screen = App::FixedSize(7, 7);
  Loop loop(&screen, component);
  loop.RunOnce();

  screen.PostEvent(MousePressed(2, 2));  // local (1, 1)
  loop.RunOnce();
  screen.PostEvent(MouseMove(4, 4));  // local (3, 3)
  loop.RunOnce();
  screen.PostEvent(MouseMove(6, 6));  // local (5, 5)
  loop.RunOnce();
  screen.PostEvent(MouseReleased(6, 6));
  loop.RunOnce();

  EXPECT_EQ(screen.GetSelection(),
            "123456\n0123456\n0123456\n0123456\n012345");

  // Cross-check what actually gets rendered for that selection: row 1 from
  // column 1 to the end, rows 2-4 fully selected, row 5 from the start to
  // column 5.
  auto element = text(content);
  auto render_screen = App::FixedSize(7, 7);
  Selection selection(1, 1, 5, 5);
  Render(render_screen, element.get(), selection);
  EXPECT_EQ(render_screen.ToString(),
            "0123456\r\n"
            "0\x1B[7m123456\x1B[27m\r\n"
            "\x1B[7m0123456\x1B[27m\r\n"
            "\x1B[7m0123456\x1B[27m\r\n"
            "\x1B[7m0123456\x1B[27m\r\n"
            "\x1B[7m012345\x1B[27m6\r\n"
            "0123456");
}

TEST(SelectionTest, HBoxSaturatedSelection) {
  auto element = hbox({
      text("Lorem ipsum dolor"),
      text("Ut enim ad minim"),
      text("Duis aute irure"),
  });

  auto screen = App::FixedSize(60, 1);

  Selection selection(2, 0, 35, 0);
  Render(screen, element.get(), selection);
  EXPECT_EQ(selection.GetParts(), "rem ipsum dolorUt enim ad minimDui");
  EXPECT_EQ(screen.ToString(),
            "Lo\x1B[7mrem ipsum dolorUt enim ad minimDui\x1B[27ms aute irure   "
            "         ");
}

}  // namespace ftxui
// NOLINTEND
