#include <utility>  // for move

#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"  // for Mouse
#include "ftxui/screen/string.hpp"    // for to_wstring

namespace ftxui {

// static
Event Event::Character(std::string input) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::Character;
  return event;
}

// static
Event Event::Character(char c) {
  return Event::Character(std::string{c});
}

// static
Event Event::Character(wchar_t c) {
  return Event::Character(to_string(std::wstring{c}));
}

// static
Event Event::Mouse(std::string input, struct Mouse mouse) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::Mouse;
  event.mouse_ = mouse;  // NOLINT
  return event;
}

// static
Event Event::Special(std::string input) {
  Event event;
  event.input_ = std::move(input);
  return event;
}

// static
Event Event::CursorReporting(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::CursorReporting;
  event.cursor_.x = x;  // NOLINT
  event.cursor_.y = y;  // NOLINT
  return event;
}

// --- Arrow ---
const Event Event::ArrowLeft = Event::Special("\x1B[D");       // NOLINT
const Event Event::ArrowRight = Event::Special("\x1B[C");      // NOLINT
const Event Event::ArrowUp = Event::Special("\x1B[A");         // NOLINT
const Event Event::ArrowDown = Event::Special("\x1B[B");       // NOLINT
const Event Event::Backspace = Event::Special({127});          // NOLINT
const Event Event::Delete = Event::Special("\x1B[3~");         // NOLINT
const Event Event::Escape = Event::Special("\x1B");            // NOLINT
const Event Event::Return = Event::Special({10});              // NOLINT
const Event Event::Tab = Event::Special({9});                  // NOLINT
const Event Event::TabReverse = Event::Special({27, 91, 90});  // NOLINT
const Event Event::F1 = Event::Special("\x1B[OP");             // NOLINT
const Event Event::F2 = Event::Special("\x1B[OQ");             // NOLINT
const Event Event::F3 = Event::Special("\x1B[OR");             // NOLINT
const Event Event::F4 = Event::Special("\x1B[OS");             // NOLINT
const Event Event::F5 = Event::Special("\x1B[15~");            // NOLINT
const Event Event::F6 = Event::Special("\x1B[17~");            // NOLINT
const Event Event::F7 = Event::Special("\x1B[18~");            // NOLINT
const Event Event::F8 = Event::Special("\x1B[19~");            // NOLINT
const Event Event::F9 = Event::Special("\x1B[20~");            // NOLINT
const Event Event::F10 = Event::Special("\x1B[21~");           // NOLINT
const Event Event::F11 = Event::Special("\x1B[21~");  // Doesn't exist // NOLINT
const Event Event::F12 = Event::Special("\x1B[24~");  // NOLINT
const Event Event::Home = Event::Special({27, 91, 72});           // NOLINT
const Event Event::End = Event::Special({27, 91, 70});            // NOLINT
const Event Event::PageUp = Event::Special({27, 91, 53, 126});    // NOLINT
const Event Event::PageDown = Event::Special({27, 91, 54, 126});  // NOLINT
const Event Event::Custom = Event::Special({0});                  // NOLINT

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
