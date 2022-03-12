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

std::string Stringify(Event event) {
  std::string out;
  for (auto& it : event.input())
    out += " " + std::to_string((unsigned int)it);

  out = "(" + out + " ) -> ";
  if (event.is_character()) {
    out += "character(" + event.character() + ")";
  } else if (event.is_mouse()) {
    out += "mouse";
    switch (event.mouse().button) {
      case Mouse::Left:
        out += "_left";
        break;
      case Mouse::Middle:
        out += "_middle";
        break;
      case Mouse::Right:
        out += "_right";
        break;
      case Mouse::None:
        out += "_none";
        break;
      case Mouse::WheelUp:
        out += "_wheel_up";
        break;
      case Mouse::WheelDown:
        out += "_wheel_down";
        break;
    }
    switch (event.mouse().motion) {
      case Mouse::Pressed:
        out += "_pressed";
        break;
      case Mouse::Released:
        out += "_released";
        break;
    }
    if (event.mouse().control)
      out += "_control";
    if (event.mouse().shift)
      out += "_shift";
    if (event.mouse().meta)
      out += "_meta";

    out += "(" +  //
           std::to_string(event.mouse().x) + "," +
           std::to_string(event.mouse().y) + ")";
  } else {
    out += "(special)";
  }
  return out;
}

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<Event> keys;

  auto component = Renderer([&] {
    Elements children;
    for (size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i)
      children.push_back(text(Stringify(keys[i])));
    return window(text("keys"), vbox(std::move(children)));
  });

  component |= CatchEvent([&](Event event) {
    keys.push_back(event);
    return true;
  });

  screen.Loop(component);
}
