#ifndef FTXUI_COMPONENT_EVENT_HPP
#define FTXUI_COMPONENT_EVENT_HPP

#include <array>
#include <ftxui/component/receiver.hpp>
#include <functional>
#include <string>
#include <vector>

namespace ftxui {

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
  static Event Character(char);
  static Event Character(wchar_t);

  static Event Character(std::string);
  static Event Special(std::string);
  static Event MouseMove(std::string, int x, int y);
  static Event MouseUp(std::string, int x, int y);
  static Event MouseLeftMove(std::string, int x, int y);
  static Event MouseLeftDown(std::string, int x, int y);
  static Event MouseMiddleMove(std::string, int x, int y);
  static Event MouseMiddleDown(std::string, int x, int y);
  static Event MouseRightMove(std::string, int x, int y);
  static Event MouseRightDown(std::string, int x, int y);

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

  // --- Custom ---
  static Event Custom;

  //--- Method section ---------------------------------------------------------
  bool is_character() const { return type_ == Type::Character;}
  wchar_t character() const { return character_; }

  bool is_mouse_left_down() const { return type_ == Type::MouseLeftDown; }
  bool is_mouse_left_move() const { return type_ == Type::MouseLeftMove; }
  bool is_mouse_middle_down() const { return type_ == Type::MouseMiddleDown; }
  bool is_mouse_middle_move() const { return type_ == Type::MouseMiddleMove; }
  bool is_mouse_right_down() const { return type_ == Type::MouseRightDown; }
  bool is_mouse_right_move() const { return type_ == Type::MouseRightMove; }
  bool is_mouse_up() const { return type_ == Type::MouseUp; }
  bool is_mouse_move() const { return type_ == Type::MouseMove; }
  int mouse_x() const { return mouse_.x; }
  int mouse_y() const { return mouse_.y; }

  const std::string& input() const { return input_; }

  bool operator==(const Event& other) const { return input_ == other.input_; }

  //--- State section ----------------------------------------------------------
 private:
  enum class Type {
    Unknown,
    Character,
    MouseMove,
    MouseUp,
    MouseLeftDown,
    MouseLeftMove,
    MouseMiddleDown,
    MouseMiddleMove,
    MouseRightDown,
    MouseRightMove,
  };

  struct Mouse {
    int x;
    int y;
  };

  Type type_ = Type::Unknown;

  union {
    wchar_t character_ = U'?';
    Mouse mouse_;
  };
  std::string input_;
};


}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_EVENT_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
