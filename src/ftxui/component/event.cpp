// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <map>      // for map
#include <utility>  // for move

#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"  // for Mouse
#include "ftxui/screen/string.hpp"    // for to_wstring

namespace ftxui {

/// @brief An event corresponding to a given typed character.
/// @param input The character typed by the user.
/// @ingroup component
// static
Event Event::Character(std::string input) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::Character;
  return event;
}

/// @brief An event corresponding to a given typed character.
/// @param c The character typed by the user.
/// @ingroup component
// static
Event Event::Character(char c) {
  return Event::Character(std::string{c});
}

/// @brief An event corresponding to a given typed character.
/// @param c The character typed by the user.
/// @ingroup component
// static
Event Event::Character(wchar_t c) {
  return Event::Character(to_string(std::wstring{c}));
}

/// @brief An event corresponding to a given typed character.
/// @param input The sequence of character send by the terminal.
/// @param mouse The mouse state.
/// @ingroup component
// static
Event Event::Mouse(std::string input, struct Mouse mouse) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::Mouse;
  event.data_.mouse = mouse;  // NOLINT
  return event;
}

/// @brief An event corresponding to a terminal DCS (Device Control String).
// static
Event Event::CursorShape(std::string input, int shape) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::CursorShape;
  event.data_.cursor_shape = shape;  // NOLINT
  return event;
}

/// @brief An custom event whose meaning is defined by the user of the library.
/// @param input An arbitrary sequence of character defined by the developer.
/// @ingroup component.
// static
Event Event::Special(std::string input) {
  Event event;
  event.input_ = std::move(input);
  return event;
}

/// @internal
// static
Event Event::CursorPosition(std::string input, int x, int y) {
  Event event;
  event.input_ = std::move(input);
  event.type_ = Type::CursorPosition;
  event.data_.cursor = {x, y};  // NOLINT
  return event;
}

/// @brief Return a string representation of the event.
std::string Event::DebugString() const {
  static std::map<Event, const char*> event_to_string = {
      // --- Arrow ---
      {Event::ArrowLeft, "Event::ArrowLeft"},
      {Event::ArrowRight, "Event::ArrowRight"},
      {Event::ArrowUp, "Event::ArrowUp"},
      {Event::ArrowDown, "Event::ArrowDown"},

      // --- ArrowCtrl ---
      {Event::ArrowLeftCtrl, "Event::ArrowLeftCtrl"},
      {Event::ArrowRightCtrl, "Event::ArrowRightCtrl"},
      {Event::ArrowUpCtrl, "Event::ArrowUpCtrl"},
      {Event::ArrowDownCtrl, "Event::ArrowDownCtrl"},

      // --- Other ---
      {Event::Backspace, "Event::Backspace"},
      {Event::Delete, "Event::Delete"},
      {Event::Escape, "Event::Escape"},
      {Event::Return, "Event::Return"},
      {Event::Tab, "Event::Tab"},
      {Event::TabReverse, "Event::TabReverse"},

      // --- Function keys ---
      {Event::F1, "Event::F1"},
      {Event::F2, "Event::F2"},
      {Event::F3, "Event::F3"},
      {Event::F4, "Event::F4"},
      {Event::F5, "Event::F5"},
      {Event::F6, "Event::F6"},
      {Event::F7, "Event::F7"},
      {Event::F8, "Event::F8"},
      {Event::F9, "Event::F9"},
      {Event::F10, "Event::F10"},
      {Event::F11, "Event::F11"},
      {Event::F12, "Event::F12"},

      // --- Navigation keys ---
      {Event::Insert, "Event::Insert"},
      {Event::Home, "Event::Home"},
      {Event::End, "Event::End"},
      {Event::PageUp, "Event::PageUp"},
      {Event::PageDown, "Event::PageDown"},

      // --- Control keys ---
      {Event::CtrlA, "Event::CtrlA"},
      {Event::CtrlB, "Event::CtrlB"},
      {Event::CtrlC, "Event::CtrlC"},
      {Event::CtrlD, "Event::CtrlD"},
      {Event::CtrlE, "Event::CtrlE"},
      {Event::CtrlF, "Event::CtrlF"},
      {Event::CtrlG, "Event::CtrlG"},
      {Event::CtrlH, "Event::CtrlH"},
      {Event::CtrlI, "Event::CtrlI"},
      {Event::CtrlJ, "Event::CtrlJ"},
      {Event::CtrlK, "Event::CtrlK"},
      {Event::CtrlL, "Event::CtrlL"},
      {Event::CtrlM, "Event::CtrlM"},
      {Event::CtrlN, "Event::CtrlN"},
      {Event::CtrlO, "Event::CtrlO"},
      {Event::CtrlP, "Event::CtrlP"},
      {Event::CtrlQ, "Event::CtrlQ"},
      {Event::CtrlR, "Event::CtrlR"},
      {Event::CtrlS, "Event::CtrlS"},
      {Event::CtrlT, "Event::CtrlT"},
      {Event::CtrlU, "Event::CtrlU"},
      {Event::CtrlV, "Event::CtrlV"},
      {Event::CtrlW, "Event::CtrlW"},
      {Event::CtrlX, "Event::CtrlX"},
      {Event::CtrlY, "Event::CtrlY"},
      {Event::CtrlZ, "Event::CtrlZ"},

      // --- Alt keys ---
      {Event::AltA, "Event::AltA"},
      {Event::AltB, "Event::AltB"},
      {Event::AltC, "Event::AltC"},
      {Event::AltD, "Event::AltD"},
      {Event::AltE, "Event::AltE"},
      {Event::AltF, "Event::AltF"},
      {Event::AltG, "Event::AltG"},
      {Event::AltH, "Event::AltH"},
      {Event::AltI, "Event::AltI"},
      {Event::AltJ, "Event::AltJ"},
      {Event::AltK, "Event::AltK"},
      {Event::AltL, "Event::AltL"},
      {Event::AltM, "Event::AltM"},
      {Event::AltN, "Event::AltN"},
      {Event::AltO, "Event::AltO"},
      {Event::AltP, "Event::AltP"},
      {Event::AltQ, "Event::AltQ"},
      {Event::AltR, "Event::AltR"},
      {Event::AltS, "Event::AltS"},
      {Event::AltT, "Event::AltT"},
      {Event::AltU, "Event::AltU"},
      {Event::AltV, "Event::AltV"},
      {Event::AltW, "Event::AltW"},
      {Event::AltX, "Event::AltX"},
      {Event::AltY, "Event::AltY"},
      {Event::AltZ, "Event::AltZ"},

      // --- CtrlAlt keys ---
      {Event::CtrlAltA, "Event::CtrlAltA"},
      {Event::CtrlAltB, "Event::CtrlAltB"},
      {Event::CtrlAltC, "Event::CtrlAltC"},
      {Event::CtrlAltD, "Event::CtrlAltD"},
      {Event::CtrlAltE, "Event::CtrlAltE"},
      {Event::CtrlAltF, "Event::CtrlAltF"},
      {Event::CtrlAltG, "Event::CtrlAltG"},
      {Event::CtrlAltH, "Event::CtrlAltH"},
      {Event::CtrlAltI, "Event::CtrlAltI"},
      {Event::CtrlAltJ, "Event::CtrlAltJ"},
      {Event::CtrlAltK, "Event::CtrlAltK"},
      {Event::CtrlAltL, "Event::CtrlAltL"},
      {Event::CtrlAltM, "Event::CtrlAltM"},
      {Event::CtrlAltN, "Event::CtrlAltN"},
      {Event::CtrlAltO, "Event::CtrlAltO"},
      {Event::CtrlAltP, "Event::CtrlAltP"},
      {Event::CtrlAltQ, "Event::CtrlAltQ"},
      {Event::CtrlAltR, "Event::CtrlAltR"},
      {Event::CtrlAltS, "Event::CtrlAltS"},
      {Event::CtrlAltT, "Event::CtrlAltT"},
      {Event::CtrlAltU, "Event::CtrlAltU"},
      {Event::CtrlAltV, "Event::CtrlAltV"},
      {Event::CtrlAltW, "Event::CtrlAltW"},
      {Event::CtrlAltX, "Event::CtrlAltX"},
      {Event::CtrlAltY, "Event::CtrlAltY"},
      {Event::CtrlAltZ, "Event::CtrlAltZ"},

      // --- Custom ---
      {Event::Custom, "Event::Custom"},
  };

  static std::map<Mouse::Button, const char*> mouse_button_string = {
      {Mouse::Button::Left, ".button = Mouse::Left"},
      {Mouse::Button::Middle, ".button = Mouse::Middle"},
      {Mouse::Button::Right, ".button = Mouse::Right"},
      {Mouse::Button::WheelUp, ".button = Mouse::WheelUp"},
      {Mouse::Button::WheelDown, ".button = Mouse::WheelDown"},
      {Mouse::Button::None, ".button = Mouse::None"},
      {Mouse::Button::WheelLeft, ".button = Mouse::WheelLeft"},
      {Mouse::Button::WheelRight, ".button = Mouse::WheelRight"},
  };

  static std::map<Mouse::Motion, const char*> mouse_motion_string = {
      {Mouse::Motion::Pressed, ".motion = Mouse::Pressed"},
      {Mouse::Motion::Released, ".motion = Mouse::Released"},
      {Mouse::Motion::Moved, ".motion = Mouse::Moved"},
  };

  switch(type_) {
    case Type::Character: {
      return "Event::Character(\"" + input_ + "\")";
    }
    case Type::Mouse: {
      std::string out = "Event::Mouse(\"...\", Mouse{";
      out += std::string(mouse_button_string[data_.mouse.button]);
      out += ", ";
      out += std::string(mouse_motion_string[data_.mouse.motion]);
      out += ", ";
      if (data_.mouse.shift) {
        out += ".shift = true, ";
      }
      if (data_.mouse.meta) {
        out += ".meta = true, ";
      }
      if (data_.mouse.control) {
        out += ".control = true, ";
      }
      out += ".x = " + std::to_string(data_.mouse.x);
      out += ", ";
      out += ".y = " + std::to_string(data_.mouse.y);
      out += "})";
      return out;
    }
    case Type::CursorShape:
      return "Event::CursorShape(" + input_ + ", " +
             std::to_string(data_.cursor_shape) + ")";
    case Type::CursorPosition:
      return "Event::CursorPosition(" + input_ + ", " +
             std::to_string(data_.cursor.x) + ", " +
             std::to_string(data_.cursor.y) + ")";
    default: {
      auto event_it = event_to_string.find(*this);
      if (event_it != event_to_string.end()) {
        return event_it->second;
      }

      return "";
    }
  }
  return "";
}

// --- Arrow ---
const Event Event::ArrowLeft = Event::Special("\x1B[D");          // NOLINT
const Event Event::ArrowRight = Event::Special("\x1B[C");         // NOLINT
const Event Event::ArrowUp = Event::Special("\x1B[A");            // NOLINT
const Event Event::ArrowDown = Event::Special("\x1B[B");          // NOLINT
const Event Event::ArrowLeftCtrl = Event::Special("\x1B[1;5D");   // NOLINT
const Event Event::ArrowRightCtrl = Event::Special("\x1B[1;5C");  // NOLINT
const Event Event::ArrowUpCtrl = Event::Special("\x1B[1;5A");     // NOLINT
const Event Event::ArrowDownCtrl = Event::Special("\x1B[1;5B");   // NOLINT
const Event Event::Backspace = Event::Special({127});             // NOLINT
const Event Event::Delete = Event::Special("\x1B[3~");            // NOLINT
const Event Event::Escape = Event::Special("\x1B");               // NOLINT
const Event Event::Return = Event::Special({10});                 // NOLINT
const Event Event::Tab = Event::Special({9});                     // NOLINT
const Event Event::TabReverse = Event::Special({27, 91, 90});     // NOLINT

// See https://invisible-island.net/xterm/xterm-function-keys.html
// We follow xterm-new / vterm-xf86-v4 / mgt / screen
const Event Event::F1 = Event::Special("\x1BOP");     // NOLINT
const Event Event::F2 = Event::Special("\x1BOQ");     // NOLINT
const Event Event::F3 = Event::Special("\x1BOR");     // NOLINT
const Event Event::F4 = Event::Special("\x1BOS");     // NOLINT
const Event Event::F5 = Event::Special("\x1B[15~");   // NOLINT
const Event Event::F6 = Event::Special("\x1B[17~");   // NOLINT
const Event Event::F7 = Event::Special("\x1B[18~");   // NOLINT
const Event Event::F8 = Event::Special("\x1B[19~");   // NOLINT
const Event Event::F9 = Event::Special("\x1B[20~");   // NOLINT
const Event Event::F10 = Event::Special("\x1B[21~");  // NOLINT
const Event Event::F11 = Event::Special("\x1B[23~");  // NOLINT
const Event Event::F12 = Event::Special("\x1B[24~");  // NOLINT

const Event Event::Insert = Event::Special("\x1B[2~");            // NOLINT
const Event Event::Home = Event::Special({27, 91, 72});           // NOLINT
const Event Event::End = Event::Special({27, 91, 70});            // NOLINT
const Event Event::PageUp = Event::Special({27, 91, 53, 126});    // NOLINT
const Event Event::PageDown = Event::Special({27, 91, 54, 126});  // NOLINT
const Event Event::Custom = Event::Special({0});                  // NOLINT
                                                                  //
const Event Event::a = Event::Character("a"); // NOLINT
const Event Event::b = Event::Character("b"); // NOLINT
const Event Event::c = Event::Character("c"); // NOLINT
const Event Event::d = Event::Character("d"); // NOLINT
const Event Event::e = Event::Character("e"); // NOLINT
const Event Event::f = Event::Character("f"); // NOLINT
const Event Event::g = Event::Character("g"); // NOLINT
const Event Event::h = Event::Character("h"); // NOLINT
const Event Event::i = Event::Character("i"); // NOLINT
const Event Event::j = Event::Character("j"); // NOLINT
const Event Event::k = Event::Character("k"); // NOLINT
const Event Event::l = Event::Character("l"); // NOLINT
const Event Event::m = Event::Character("m"); // NOLINT
const Event Event::n = Event::Character("n"); // NOLINT
const Event Event::o = Event::Character("o"); // NOLINT
const Event Event::p = Event::Character("p"); // NOLINT
const Event Event::q = Event::Character("q"); // NOLINT
const Event Event::r = Event::Character("r"); // NOLINT
const Event Event::s = Event::Character("s"); // NOLINT
const Event Event::t = Event::Character("t"); // NOLINT
const Event Event::u = Event::Character("u"); // NOLINT
const Event Event::v = Event::Character("v"); // NOLINT
const Event Event::w = Event::Character("w"); // NOLINT
const Event Event::x = Event::Character("x"); // NOLINT
const Event Event::y = Event::Character("y"); // NOLINT
const Event Event::z = Event::Character("z"); // NOLINT

const Event Event::A = Event::Character("A"); // NOLINT
const Event Event::B = Event::Character("B"); // NOLINT
const Event Event::C = Event::Character("C"); // NOLINT
const Event Event::D = Event::Character("D"); // NOLINT
const Event Event::E = Event::Character("E"); // NOLINT
const Event Event::F = Event::Character("F"); // NOLINT
const Event Event::G = Event::Character("G"); // NOLINT
const Event Event::H = Event::Character("H"); // NOLINT
const Event Event::I = Event::Character("I"); // NOLINT
const Event Event::J = Event::Character("J"); // NOLINT
const Event Event::K = Event::Character("K"); // NOLINT
const Event Event::L = Event::Character("L"); // NOLINT
const Event Event::M = Event::Character("M"); // NOLINT
const Event Event::N = Event::Character("N"); // NOLINT
const Event Event::O = Event::Character("O"); // NOLINT
const Event Event::P = Event::Character("P"); // NOLINT
const Event Event::Q = Event::Character("Q"); // NOLINT
const Event Event::R = Event::Character("R"); // NOLINT
const Event Event::S = Event::Character("S"); // NOLINT
const Event Event::T = Event::Character("T"); // NOLINT
const Event Event::U = Event::Character("U"); // NOLINT
const Event Event::V = Event::Character("V"); // NOLINT
const Event Event::W = Event::Character("W"); // NOLINT
const Event Event::X = Event::Character("X"); // NOLINT
const Event Event::Y = Event::Character("Y"); // NOLINT
const Event Event::Z = Event::Character("Z"); // NOLINT

const Event Event::CtrlA = Event::Special("\x01"); // NOLINT
const Event Event::CtrlB = Event::Special("\x02"); // NOLINT
const Event Event::CtrlC = Event::Special("\x03"); // NOLINT
const Event Event::CtrlD = Event::Special("\x04"); // NOLINT
const Event Event::CtrlE = Event::Special("\x05"); // NOLINT
const Event Event::CtrlF = Event::Special("\x06"); // NOLINT
const Event Event::CtrlG = Event::Special("\x07"); // NOLINT
const Event Event::CtrlH = Event::Special("\x08"); // NOLINT
const Event Event::CtrlI = Event::Special("\x09"); // NOLINT
const Event Event::CtrlJ = Event::Special("\x0a"); // NOLINT
const Event Event::CtrlK = Event::Special("\x0b"); // NOLINT
const Event Event::CtrlL = Event::Special("\x0c"); // NOLINT
const Event Event::CtrlM = Event::Special("\x0d"); // NOLINT
const Event Event::CtrlN = Event::Special("\x0e"); // NOLINT
const Event Event::CtrlO = Event::Special("\x0f"); // NOLINT
const Event Event::CtrlP = Event::Special("\x10"); // NOLINT
const Event Event::CtrlQ = Event::Special("\x11"); // NOLINT
const Event Event::CtrlR = Event::Special("\x12"); // NOLINT
const Event Event::CtrlS = Event::Special("\x13"); // NOLINT
const Event Event::CtrlT = Event::Special("\x14"); // NOLINT
const Event Event::CtrlU = Event::Special("\x15"); // NOLINT
const Event Event::CtrlV = Event::Special("\x16"); // NOLINT
const Event Event::CtrlW = Event::Special("\x17"); // NOLINT
const Event Event::CtrlX = Event::Special("\x18"); // NOLINT
const Event Event::CtrlY = Event::Special("\x19"); // NOLINT
const Event Event::CtrlZ = Event::Special("\x1a"); // NOLINT

const Event Event::AltA = Event::Special("\x1b""a"); // NOLINT
const Event Event::AltB = Event::Special("\x1b""b"); // NOLINT
const Event Event::AltC = Event::Special("\x1b""c"); // NOLINT
const Event Event::AltD = Event::Special("\x1b""d"); // NOLINT
const Event Event::AltE = Event::Special("\x1b""e"); // NOLINT
const Event Event::AltF = Event::Special("\x1b""f"); // NOLINT
const Event Event::AltG = Event::Special("\x1b""g"); // NOLINT
const Event Event::AltH = Event::Special("\x1b""h"); // NOLINT
const Event Event::AltI = Event::Special("\x1b""i"); // NOLINT
const Event Event::AltJ = Event::Special("\x1b""j"); // NOLINT
const Event Event::AltK = Event::Special("\x1b""k"); // NOLINT
const Event Event::AltL = Event::Special("\x1b""l"); // NOLINT
const Event Event::AltM = Event::Special("\x1b""m"); // NOLINT
const Event Event::AltN = Event::Special("\x1b""n"); // NOLINT
const Event Event::AltO = Event::Special("\x1b""o"); // NOLINT
const Event Event::AltP = Event::Special("\x1b""p"); // NOLINT
const Event Event::AltQ = Event::Special("\x1b""q"); // NOLINT
const Event Event::AltR = Event::Special("\x1b""r"); // NOLINT
const Event Event::AltS = Event::Special("\x1b""s"); // NOLINT
const Event Event::AltT = Event::Special("\x1b""t"); // NOLINT
const Event Event::AltU = Event::Special("\x1b""u"); // NOLINT
const Event Event::AltV = Event::Special("\x1b""v"); // NOLINT
const Event Event::AltW = Event::Special("\x1b""w"); // NOLINT
const Event Event::AltX = Event::Special("\x1b""x"); // NOLINT
const Event Event::AltY = Event::Special("\x1b""y"); // NOLINT
const Event Event::AltZ = Event::Special("\x1b""z"); // NOLINT

const Event Event::CtrlAltA = Event::Special("\x1b\x01"); // NOLINT
const Event Event::CtrlAltB = Event::Special("\x1b\x02"); // NOLINT
const Event Event::CtrlAltC = Event::Special("\x1b\x03"); // NOLINT
const Event Event::CtrlAltD = Event::Special("\x1b\x04"); // NOLINT
const Event Event::CtrlAltE = Event::Special("\x1b\x05"); // NOLINT
const Event Event::CtrlAltF = Event::Special("\x1b\x06"); // NOLINT
const Event Event::CtrlAltG = Event::Special("\x1b\x07"); // NOLINT
const Event Event::CtrlAltH = Event::Special("\x1b\x08"); // NOLINT
const Event Event::CtrlAltI = Event::Special("\x1b\x09"); // NOLINT
const Event Event::CtrlAltJ = Event::Special("\x1b\x0a"); // NOLINT
const Event Event::CtrlAltK = Event::Special("\x1b\x0b"); // NOLINT
const Event Event::CtrlAltL = Event::Special("\x1b\x0c"); // NOLINT
const Event Event::CtrlAltM = Event::Special("\x1b\x0d"); // NOLINT
const Event Event::CtrlAltN = Event::Special("\x1b\x0e"); // NOLINT
const Event Event::CtrlAltO = Event::Special("\x1b\x0f"); // NOLINT
const Event Event::CtrlAltP = Event::Special("\x1b\x10"); // NOLINT
const Event Event::CtrlAltQ = Event::Special("\x1b\x11"); // NOLINT
const Event Event::CtrlAltR = Event::Special("\x1b\x12"); // NOLINT
const Event Event::CtrlAltS = Event::Special("\x1b\x13"); // NOLINT
const Event Event::CtrlAltT = Event::Special("\x1b\x14"); // NOLINT
const Event Event::CtrlAltU = Event::Special("\x1b\x15"); // NOLINT
const Event Event::CtrlAltV = Event::Special("\x1b\x16"); // NOLINT
const Event Event::CtrlAltW = Event::Special("\x1b\x17"); // NOLINT
const Event Event::CtrlAltX = Event::Special("\x1b\x18"); // NOLINT
const Event Event::CtrlAltY = Event::Special("\x1b\x19"); // NOLINT
const Event Event::CtrlAltZ = Event::Special("\x1b\x1a"); // NOLINT

}  // namespace ftxui
