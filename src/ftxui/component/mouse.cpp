// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/mouse.hpp"

namespace ftxui {

namespace {
bool IsDown(const Mouse& mouse, Mouse::Button button) {
  return mouse.button == button && mouse.motion == Mouse::Pressed;
}
}  // namespace


/// Return whether the mouse transitionned from released to pressed.
/// This is useful to detect a click.
/// @arg button The button to check.
bool Mouse::IsPressed(Button button) const {
  return IsDown(*this, button) &&
         (!previous || !IsDown(*previous, button));
}

/// Return whether the mouse is currently held.
/// This is useful to detect a drag.
/// @arg button The button to check.
bool Mouse::IsHeld(Button button) const {
  return IsDown(*this, button) && previous && IsDown(*previous, button);
}

/// Return whether the mouse transitionned from pressed to released.
/// This is useful to detect a click.
/// @arg button The button to check.
bool Mouse::IsReleased(Button button) const {
  return !IsDown(*this, button) &&
         (previous && IsDown(*previous, button));
}

}  // namespace ftxui
