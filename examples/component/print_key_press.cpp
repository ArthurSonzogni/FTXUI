// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <stddef.h>   // for size_t
#include <algorithm>  // for max
#include <memory>     // for allocator, shared_ptr
#include <string>  // for char_traits, operator+, string, basic_string, to_string
#include <utility>  // for move
#include <vector>   // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for CatchEvent, Renderer
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Middle, Mouse::None, Mouse::Pressed, Mouse::Released, Mouse::Right, Mouse::WheelDown, Mouse::WheelUp
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, vbox, window, Element, Elements

using namespace ftxui;

std::string Code(Event event) {
  std::string codes;
  for (auto& it : event.input()) {
    codes += " " + std::to_string((unsigned int)it);
  }
  return codes;
}

int main() {
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<Event> keys;

  auto left_column = Renderer([&] {
    Elements children = {
        text("Codes"),
        separator(),
    };
    for (size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
      children.push_back(text(Code(keys[i])));
    }
    return vbox(children);
  });

  auto right_column = Renderer([&] {
    Elements children = {
        text("Event"),
        separator(),
    };
    for (size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
      children.push_back(text(keys[i].DebugString()));
    }
    return vbox(children);
  });

  int split_size = 40;
  auto component = ResizableSplitLeft(left_column, right_column, &split_size);
  component |= border;

  component |= CatchEvent([&](Event event) {
    keys.push_back(event);
    return false;
  });

  screen.Loop(component);
}
