// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stdlib.h>                   // for EXIT_SUCCESS
#include <chrono>                     // for milliseconds
#include <ftxui/component/event.hpp>  // for Event
#include <ftxui/component/mouse.hpp>  // for ftxui
#include <ftxui/dom/elements.hpp>  // for text, separator, Element, operator|, vbox, border
#include <memory>                  // for allocator, shared_ptr
#include <string>                  // for operator+, to_string
#include <thread>                  // for sleep_for

#include "ftxui/component/component.hpp"  // for CatchEvent, Renderer, operator|=
#include "ftxui/component/loop.hpp"       // for Loop
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main() {
  using namespace ftxui;
  auto screen = ScreenInteractive::FitComponent();

  // Create a component counting the number of frames drawn and event handled.
  int custom_loop_count = 0;
  int frame_count = 0;
  int event_count = 0;
  auto component = Renderer([&] {
    frame_count++;
    return vbox({
               text("This demonstrates using a custom ftxui::Loop. It "),
               text("runs at 100 iterations per seconds. The FTXUI events "),
               text("are all processed once per iteration and a new frame "),
               text("is rendered as needed"),
               separator(),
               text("ftxui event count: " + std::to_string(event_count)),
               text("ftxui frame count: " + std::to_string(frame_count)),
               text("Custom loop count: " + std::to_string(custom_loop_count)),
           }) |
           border;
  });

  component |= CatchEvent([&](Event) -> bool {
    event_count++;
    return false;
  });

  Loop loop(&screen, component);

  while (!loop.HasQuitted()) {
    custom_loop_count++;
    loop.RunOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return EXIT_SUCCESS;
}
