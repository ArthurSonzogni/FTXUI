// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/mouse.hpp"

namespace ftxui {

namespace {
bool IsDown(const Mouse* mouse, Mouse::Button btn) {
  return mouse->button == btn && mouse->motion == Mouse::Pressed;
}
}  // namespace

/// Return whether the mouse transitionned from released to pressed.
/// This is useful to detect a click.
/// @arg btn The button to check.
bool Mouse::IsPressed(Button btn) const {
  return IsDown(this, btn) && (!previous || !IsDown(previous, btn));
}

/// Return whether the mouse is currently held.
/// This is useful to detect a drag.
/// @arg btn The button to check.
bool Mouse::IsHeld(Button btn) const {
  return IsDown(this, btn) && previous && IsDown(previous, btn);
}

/// Return whether the mouse transitionned from pressed to released.
/// This is useful to detect a click.
/// @arg btn The button to check.
bool Mouse::IsReleased(Button btn) const {
  return !IsDown(this, btn) && (previous && IsDown(previous, btn));
}

}  // namespace ftxui
