#include <functional>  // for function
#include <memory>      // for shared_ptr
#include <utility>     // for move

#include "ftxui/component/captured_mouse.hpp"     // for CapturedMouse
#include "ftxui/component/component.hpp"          // for Make, Button
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption
#include "ftxui/component/event.hpp"              // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Element, nothing, reflect, text, border, inverted
#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/util/ref.hpp"    // for ConstStringRef, Ref

namespace ftxui {

namespace {
class ButtonBase : public ComponentBase {
 public:
  ButtonBase(ConstStringRef label,
             std::function<void()> on_click,
             Ref<ButtonOption> option)
      : label_(label), on_click_(on_click), option_(std::move(option)) {}

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

  bool Focusable() const final { return true; }

 private:
  ConstStringRef label_;
  std::function<void()> on_click_;
  Box box_;
  Ref<ButtonOption> option_;
};

}  // namespace

/// @brief Draw a button. Execute a function when clicked.
/// @param label The label of the button.
/// @param on_click The action to execute when clicked.
/// @param option Additional optional parameters.
/// @ingroup component
/// @see ButtonBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::string label = "Click to quit";
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
                 Ref<ButtonOption> option) {
  return Make<ButtonBase>(label, std::move(on_click), std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
