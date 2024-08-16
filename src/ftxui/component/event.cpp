// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <map>  // for map
#include <string>
#include <utility>  // for move

#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"  // for Mouse
#include "ftxui/screen/string.hpp"    // for to_wstring

// Disable warning for shadowing variable, for every compilers. Indeed, there is
// a static Event for every letter of the alphabet:
#ifdef __clang__
#pragma clang diagnostic ignored "-Wshadow"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wshadow"
#elif defined(_MSC_VER)
#pragma warning(disable : 6244)
#pragma warning(disable : 6246)
#endif

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

  switch (type_) {
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

// clang-format off
// NOLINTBEGIN

// --- Arrow ---
const Event Event::ArrowLeft      = Event::Special("\x1B[D");
const Event Event::ArrowRight     = Event::Special("\x1B[C");
const Event Event::ArrowUp        = Event::Special("\x1B[A");
const Event Event::ArrowDown      = Event::Special("\x1B[B");
const Event Event::ArrowLeftCtrl  = Event::Special("\x1B[1;5D");
const Event Event::ArrowRightCtrl = Event::Special("\x1B[1;5C");
const Event Event::ArrowUpCtrl    = Event::Special("\x1B[1;5A");
const Event Event::ArrowDownCtrl  = Event::Special("\x1B[1;5B");
const Event Event::Backspace      = Event::Special({127});
const Event Event::Delete         = Event::Special("\x1B[3~");
const Event Event::Escape         = Event::Special("\x1B");
const Event Event::Return         = Event::Special({10});
const Event Event::Tab            = Event::Special({9});
const Event Event::TabReverse     = Event::Special({27, 91, 90});

// See https://invisible-island.net/xterm/xterm-function-keys.html
// We follow xterm-new / vterm-xf86-v4 / mgt / screen
const Event Event::F1  = Event::Special("\x1BOP");
const Event Event::F2  = Event::Special("\x1BOQ");
const Event Event::F3  = Event::Special("\x1BOR");
const Event Event::F4  = Event::Special("\x1BOS");
const Event Event::F5  = Event::Special("\x1B[15~");
const Event Event::F6  = Event::Special("\x1B[17~");
const Event Event::F7  = Event::Special("\x1B[18~");
const Event Event::F8  = Event::Special("\x1B[19~");
const Event Event::F9  = Event::Special("\x1B[20~");
const Event Event::F10 = Event::Special("\x1B[21~");
const Event Event::F11 = Event::Special("\x1B[23~");
const Event Event::F12 = Event::Special("\x1B[24~");

const Event Event::Insert   = Event::Special("\x1B[2~");
const Event Event::Home     = Event::Special({27, 91, 72});
const Event Event::End      = Event::Special({27, 91, 70});
const Event Event::PageUp   = Event::Special({27, 91, 53, 126});
const Event Event::PageDown = Event::Special({27, 91, 54, 126});
const Event Event::Custom   = Event::Special({0});

const Event Event::a = Event::Character("a");
const Event Event::b = Event::Character("b");
const Event Event::c = Event::Character("c");
const Event Event::d = Event::Character("d");
const Event Event::e = Event::Character("e");
const Event Event::f = Event::Character("f");
const Event Event::g = Event::Character("g");
const Event Event::h = Event::Character("h");
const Event Event::i = Event::Character("i");
const Event Event::j = Event::Character("j");
const Event Event::k = Event::Character("k");
const Event Event::l = Event::Character("l");
const Event Event::m = Event::Character("m");
const Event Event::n = Event::Character("n");
const Event Event::o = Event::Character("o");
const Event Event::p = Event::Character("p");
const Event Event::q = Event::Character("q");
const Event Event::r = Event::Character("r");
const Event Event::s = Event::Character("s");
const Event Event::t = Event::Character("t");
const Event Event::u = Event::Character("u");
const Event Event::v = Event::Character("v");
const Event Event::w = Event::Character("w");
const Event Event::x = Event::Character("x");
const Event Event::y = Event::Character("y");
const Event Event::z = Event::Character("z");

const Event Event::A = Event::Character("A");
const Event Event::B = Event::Character("B");
const Event Event::C = Event::Character("C");
const Event Event::D = Event::Character("D");
const Event Event::E = Event::Character("E");
const Event Event::F = Event::Character("F");
const Event Event::G = Event::Character("G");
const Event Event::H = Event::Character("H");
const Event Event::I = Event::Character("I");
const Event Event::J = Event::Character("J");
const Event Event::K = Event::Character("K");
const Event Event::L = Event::Character("L");
const Event Event::M = Event::Character("M");
const Event Event::N = Event::Character("N");
const Event Event::O = Event::Character("O");
const Event Event::P = Event::Character("P");
const Event Event::Q = Event::Character("Q");
const Event Event::R = Event::Character("R");
const Event Event::S = Event::Character("S");
const Event Event::T = Event::Character("T");
const Event Event::U = Event::Character("U");
const Event Event::V = Event::Character("V");
const Event Event::W = Event::Character("W");
const Event Event::X = Event::Character("X");
const Event Event::Y = Event::Character("Y");
const Event Event::Z = Event::Character("Z");

const Event Event::CtrlA = Event::Special("\x01");
const Event Event::CtrlB = Event::Special("\x02");
const Event Event::CtrlC = Event::Special("\x03");
const Event Event::CtrlD = Event::Special("\x04");
const Event Event::CtrlE = Event::Special("\x05");
const Event Event::CtrlF = Event::Special("\x06");
const Event Event::CtrlG = Event::Special("\x07");
const Event Event::CtrlH = Event::Special("\x08");
const Event Event::CtrlI = Event::Special("\x09");
const Event Event::CtrlJ = Event::Special("\x0a");
const Event Event::CtrlK = Event::Special("\x0b");
const Event Event::CtrlL = Event::Special("\x0c");
const Event Event::CtrlM = Event::Special("\x0d");
const Event Event::CtrlN = Event::Special("\x0e");
const Event Event::CtrlO = Event::Special("\x0f");
const Event Event::CtrlP = Event::Special("\x10");
const Event Event::CtrlQ = Event::Special("\x11");
const Event Event::CtrlR = Event::Special("\x12");
const Event Event::CtrlS = Event::Special("\x13");
const Event Event::CtrlT = Event::Special("\x14");
const Event Event::CtrlU = Event::Special("\x15");
const Event Event::CtrlV = Event::Special("\x16");
const Event Event::CtrlW = Event::Special("\x17");
const Event Event::CtrlX = Event::Special("\x18");
const Event Event::CtrlY = Event::Special("\x19");
const Event Event::CtrlZ = Event::Special("\x1a");

const Event Event::AltA = Event::Special("\x1b""a");
const Event Event::AltB = Event::Special("\x1b""b");
const Event Event::AltC = Event::Special("\x1b""c");
const Event Event::AltD = Event::Special("\x1b""d");
const Event Event::AltE = Event::Special("\x1b""e");
const Event Event::AltF = Event::Special("\x1b""f");
const Event Event::AltG = Event::Special("\x1b""g");
const Event Event::AltH = Event::Special("\x1b""h");
const Event Event::AltI = Event::Special("\x1b""i");
const Event Event::AltJ = Event::Special("\x1b""j");
const Event Event::AltK = Event::Special("\x1b""k");
const Event Event::AltL = Event::Special("\x1b""l");
const Event Event::AltM = Event::Special("\x1b""m");
const Event Event::AltN = Event::Special("\x1b""n");
const Event Event::AltO = Event::Special("\x1b""o");
const Event Event::AltP = Event::Special("\x1b""p");
const Event Event::AltQ = Event::Special("\x1b""q");
const Event Event::AltR = Event::Special("\x1b""r");
const Event Event::AltS = Event::Special("\x1b""s");
const Event Event::AltT = Event::Special("\x1b""t");
const Event Event::AltU = Event::Special("\x1b""u");
const Event Event::AltV = Event::Special("\x1b""v");
const Event Event::AltW = Event::Special("\x1b""w");
const Event Event::AltX = Event::Special("\x1b""x");
const Event Event::AltY = Event::Special("\x1b""y");
const Event Event::AltZ = Event::Special("\x1b""z");

const Event Event::CtrlAltA = Event::Special("\x1b\x01");
const Event Event::CtrlAltB = Event::Special("\x1b\x02");
const Event Event::CtrlAltC = Event::Special("\x1b\x03");
const Event Event::CtrlAltD = Event::Special("\x1b\x04");
const Event Event::CtrlAltE = Event::Special("\x1b\x05");
const Event Event::CtrlAltF = Event::Special("\x1b\x06");
const Event Event::CtrlAltG = Event::Special("\x1b\x07");
const Event Event::CtrlAltH = Event::Special("\x1b\x08");
const Event Event::CtrlAltI = Event::Special("\x1b\x09");
const Event Event::CtrlAltJ = Event::Special("\x1b\x0a");
const Event Event::CtrlAltK = Event::Special("\x1b\x0b");
const Event Event::CtrlAltL = Event::Special("\x1b\x0c");
const Event Event::CtrlAltM = Event::Special("\x1b\x0d");
const Event Event::CtrlAltN = Event::Special("\x1b\x0e");
const Event Event::CtrlAltO = Event::Special("\x1b\x0f");
const Event Event::CtrlAltP = Event::Special("\x1b\x10");
const Event Event::CtrlAltQ = Event::Special("\x1b\x11");
const Event Event::CtrlAltR = Event::Special("\x1b\x12");
const Event Event::CtrlAltS = Event::Special("\x1b\x13");
const Event Event::CtrlAltT = Event::Special("\x1b\x14");
const Event Event::CtrlAltU = Event::Special("\x1b\x15");
const Event Event::CtrlAltV = Event::Special("\x1b\x16");
const Event Event::CtrlAltW = Event::Special("\x1b\x17");
const Event Event::CtrlAltX = Event::Special("\x1b\x18");
const Event Event::CtrlAltY = Event::Special("\x1b\x19");
const Event Event::CtrlAltZ = Event::Special("\x1b\x1a");

// NOLINTEND
// clang-format on

}  // namespace ftxui
