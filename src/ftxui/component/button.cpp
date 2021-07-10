#include <functional>  // for function
#include <memory>      // for shared_ptr

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for CapturedMouse
#include "ftxui/component/event.hpp"           // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

namespace ftxui {

namespace {
/// @brief A button. An action is associated to the click event.
/// @ingroup dom
class ButtonBase : public ComponentBase {
 public:
  ButtonBase(ConstStringRef label,
             std::function<void()> on_click,
             ConstRef<ButtonOption> option)
      : label_(label), on_click_(on_click), option_(std::move(option)) {}

  ~ButtonBase() override = default;

  // Component implementation:
  Element Render() override {
    auto style = Focused() ? inverted : nothing;
    auto my_border = option_->border ? border : nothing;
    return text(*label_) | my_border | style | reflect(box_);
  }

  bool OnEvent(Event event) override {
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

 private:
  ConstStringRef label_;
  std::function<void()> on_click_;
  Box box_;
  ConstRef<ButtonOption> option_;
};

}  // namespace

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
Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 ConstRef<ButtonOption> option) {
  return Make<ButtonBase>(label, std::move(on_click), std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
