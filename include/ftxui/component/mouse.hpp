// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_MOUSE_HPP
#define FTXUI_COMPONENT_MOUSE_HPP
namespace ftxui {

/// @brief A mouse event. It contains the coordinate of the mouse, the button
/// pressed and the modifier (shift, ctrl, meta).
/// @ingroup component
struct Mouse {
  enum Button {
    Left = 0,
    Middle = 1,
    Right = 2,
    None = 3,
    WheelUp = 4,
    WheelDown = 5,
  };

  enum Motion {
    Released = 0,
    Pressed = 1,
  };

  // Button
  Button button = Button::None;

  // Motion
  Motion motion = Motion::Pressed;

  // Modifiers:
  bool shift = false;
  bool meta = false;
  bool control = false;

  // Coordinates:
  int x = 0;
  int y = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_MOUSE_HPP */
