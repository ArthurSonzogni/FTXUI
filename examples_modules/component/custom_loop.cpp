#include <typeinfo>
#include <initializer_list>
#include <chrono>                     // for milliseconds
#include <memory>                  // for allocator, shared_ptr
#include <stdlib.h>                   // for EXIT_SUCCESS
#include <string>                  // for operator+, to_string
#include <thread>                  // for sleep_for
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;
  auto screen = App::FitComponent();

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
