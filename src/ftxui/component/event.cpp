#include <utility>  // for move

#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"  // for Mouse
#include "ftxui/screen/string.hpp"    // for to_wstring

namespace ftxui {

// static
Event Event::Character(std::string input) {
  Event event;
  event.character_ = to_wstring(input)[0];
  event.input_ = std::move(input);
  event.type_ = Type::Character;
  return event;
}

// static
Event Event::Character(char c) {
  return Character(wchar_t(c));
}

// static
Event Event::Character(wchar_t c) {
  Event event;
  event.input_ = {(char)c};
  event.type_ = Type::Character;
  event.character_ = c;
  return event;
}

// static
Event Event::Mouse(std::string input, struct Mouse mouse) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::Mouse;
  event.mouse_ = mouse;
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
  event.cursor_.x = x;
  event.cursor_.y = y;
  return event;
}

// --- Arrow ---
const Event Event::ArrowLeft = Event::Special("\x1B[D");
const Event Event::ArrowRight = Event::Special("\x1B[C");
const Event Event::ArrowUp = Event::Special("\x1B[A");
const Event Event::ArrowDown = Event::Special("\x1B[B");
const Event Event::Backspace = Event::Special({127});
const Event Event::Delete = Event::Special("\x1B[3~");
const Event Event::Escape = Event::Special("\x1B");
#if defined(_WIN32)
const Event Event::Return = Event::Special({13});
#else
const Event Event::Return = Event::Special({10});
#endif
const Event Event::Tab = Event::Special({9});
const Event Event::TabReverse = Event::Special({27, 91, 90});
const Event Event::F1 = Event::Special("\x1B[OP");
const Event Event::F2 = Event::Special("\x1B[OQ");
const Event Event::F3 = Event::Special("\x1B[OR");
const Event Event::F4 = Event::Special("\x1B[OS");
const Event Event::F5 = Event::Special("\x1B[15~");
const Event Event::F6 = Event::Special("\x1B[17~");
const Event Event::F7 = Event::Special("\x1B[18~");
const Event Event::F8 = Event::Special("\x1B[19~");
const Event Event::F9 = Event::Special("\x1B[20~");
const Event Event::F10 = Event::Special("\x1B[21~");
const Event Event::F11 = Event::Special("\x1B[21~");  // Doesn't exist
const Event Event::F12 = Event::Special("\x1B[24~");
const Event Event::Home = Event::Special({27, 91, 72});
const Event Event::End = Event::Special({27, 91, 70});
const Event Event::PageUp = Event::Special({27, 91, 53, 126});
const Event Event::PageDown = Event::Special({27, 91, 54, 126});

Event Event::Custom = Event::Special({0});

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
