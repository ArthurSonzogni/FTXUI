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
//
// Documentation:
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
//
struct Event {
  // --- Constructor section ---------------------------------------------------
  static Event Character(char);
  static Event Character(wchar_t);

  static Event Character(const std::string&);
  static Event Special(const std::string&);

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
  bool is_character() const { return is_character_; }
  wchar_t character() const { return character_; }
  const std::string& input() const { return input_; }

  bool operator==(const Event& other) const { return input_ == other.input_; }

  //--- State section ----------------------------------------------------------
 private:
  std::string input_;
  bool is_character_ = false;
  wchar_t character_ = U'?';
};


}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_EVENT_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
