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


TEST(SelectionTest, BasicSelection) {

  int selectionChangeCounter = 0;

  auto component = Renderer([&] {
      return text("Lorem ipsum dolor");
  });
  auto screen = ScreenInteractive::FixedSize(20, 1);

  screen.setSelectionOptions({
    .transform = [](Pixel& pixel) {
      pixel.inverted = true;
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

  Render(screen, component->Render());

  EXPECT_EQ(screen.ToString(), "\x1B[7mL\x1B[27morem ipsum dolor   ");

  EXPECT_STREQ(screen.GetSelectedContent(component).c_str(), "rem ipsu");
}


}  // namespace ftxui
// NOLINTEND
