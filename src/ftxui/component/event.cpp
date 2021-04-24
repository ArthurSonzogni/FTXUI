#include "ftxui/component/event.hpp"

#include <iostream>
#include "ftxui/screen/string.hpp"

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
Event Event::MouseMove(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseMove;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::MouseUp(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseUp;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::MouseLeftDown(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseLeftDown;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::MouseLeftMove(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseLeftMove;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::MouseRightDown(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseRightDown;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::MouseMiddleMove(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseMiddleMove;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::Special(std::string input) {
  Event event;
  event.input_ = std::move(input);
  return event;
}

// static
Event Event::MouseRightMove(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseRightMove;
  event.mouse_ = {x, y};
  return event;
}

// static
Event Event::MouseMiddleDown(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::MouseMiddleDown;
  event.mouse_ = {x, y};
  return event;
}

Event Event::CursorReporting(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::CursorReporting;
  event.mouse_ = {x, y};
  return event;
}

bool Event::is_mouse() const {
  switch (type_) {
    case Type::Unknown:
    case Type::Character:
    case Type::CursorReporting:
      return false;
    case Type::MouseMove:
    case Type::MouseUp:
    case Type::MouseLeftDown:
    case Type::MouseLeftMove:
    case Type::MouseMiddleDown:
    case Type::MouseMiddleMove:
    case Type::MouseRightDown:
    case Type::MouseRightMove:
      return true;
  };
}

void Event::MoveMouse(int dx, int dy) {
  mouse_.x += dx;
  mouse_.y += dy;
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
Event Event::Custom = Event::Special({0});

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
