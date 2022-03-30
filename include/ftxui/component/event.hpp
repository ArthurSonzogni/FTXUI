#ifndef FTXUI_COMPONENT_EVENT_HPP
#define FTXUI_COMPONENT_EVENT_HPP

#include <ftxui/component/mouse.hpp>  // for Mouse
#include <functional>
#include <string>  // for string, operator==
#include <vector>

namespace ftxui {

class ScreenInteractive;
class ComponentBase;

/// @brief Represent an event. It can be key press event, a terminal resize, or
/// more ...
///
/// For example:
/// - Printable character can be created using Event::Character('a').
/// - Some special are predefined, like Event::ArrowLeft.
/// - One can find arbitrary code for special Events using:
///   ./example/util/print_key_press
///  For instance, CTLR+A maps to Event::Special({1});
///
/// Useful documentation about xterm specification:
/// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
struct Event {
  // --- Constructor section ---------------------------------------------------
  static Event Character(std::string);
  static Event Character(char);
  static Event Character(wchar_t);
  static Event Special(std::string);
  static Event Mouse(std::string, Mouse mouse);
  static Event CursorReporting(std::string, int x, int y);

  // --- Arrow ---
  static const Event ArrowLeft;
  static const Event ArrowRight;
  static const Event ArrowUp;
  static const Event ArrowDown;

  // --- Other ---
  static const Event Backspace;
  static const Event Delete;
  static const Event Return;
  static const Event Escape;
  static const Event Tab;
  static const Event TabReverse;
  static const Event F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12;

  static const Event Home;
  static const Event End;

  static const Event PageUp;
  static const Event PageDown;

  // --- Custom ---
  static const Event Custom;

  //--- Method section ---------------------------------------------------------
  bool is_character() const { return type_ == Type::Character; }
  std::string character() const { return input_; }

  bool is_mouse() const { return type_ == Type::Mouse; }
  struct Mouse& mouse() {
    return mouse_;
  }

  bool is_cursor_reporting() const { return type_ == Type::CursorReporting; }
  int cursor_x() const { return cursor_.x; }
  int cursor_y() const { return cursor_.y; }

  const std::string& input() const { return input_; }

  bool operator==(const Event& other) const { return input_ == other.input_; }
  bool operator!=(const Event& other) const { return !operator==(other); }

  //--- State section ----------------------------------------------------------
  ScreenInteractive* screen_ = nullptr;

 private:
  friend ComponentBase;
  friend ScreenInteractive;
  enum class Type {
    Unknown,
    Character,
    Mouse,
    CursorReporting,
  };
  Type type_ = Type::Unknown;

  struct Cursor {
    int x;
    int y;
  };

  union {
    struct Mouse mouse_;
    struct Cursor cursor_;
  };
  std::string input_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_EVENT_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
