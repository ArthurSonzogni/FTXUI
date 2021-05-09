#include <functional>  // for function
#include <memory>      // for shared_ptr

#include "ftxui/component/button.hpp"
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"           // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

namespace ftxui {

/// @brief Draw a button. Execute a function when clicked.
/// @param label The label of the button.
/// @param on_click The action to execute when clicked.
/// @ingroup component
/// @see ButtonBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::wstring label = L"Click to quit";
/// Component button = Button(&label, screen.ExitLoopClosure());
/// screen.Loop(button)
/// ```
///
/// ### Output
///
/// ```bash
/// ┌─────────────┐
/// │Click to quit│
/// └─────────────┘
/// ```
Component Button(const std::wstring* label, std::function<void()> on_click) {
  return Make<ButtonBase>(label, on_click);
}

// static
ButtonBase* ButtonBase::From(Component component) {
  return static_cast<ButtonBase*>(component.get());
}

ButtonBase::ButtonBase(const std::wstring* label,
                       std::function<void()> on_click)
    : label_(label), on_click_(on_click) {}

Element ButtonBase::Render() {
  auto style = Focused() ? inverted : nothing;
  return text(*label_) | border | style | reflect(box_);
}

bool ButtonBase::OnEvent(Event event) {
  if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y)) {
    if (!CaptureMouse(event))
      return false;

    TakeFocus();

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      on_click_();
      return true;
    }

    return false;
  }

  if (event == Event::Return) {
    on_click_();
    return true;
  }
  return false;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
