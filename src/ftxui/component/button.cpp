#include <functional>  // for function
#include <memory>      // for shared_ptr
#include <utility>     // for move

#include "ftxui/component/animation.hpp"  // for Animator, Params (ptr only)
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Button
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption, AnimatedColorOption, AnimatedColorsOption, EntryState
#include "ftxui/component/event.hpp"  // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Decorator, Element, operator|=, bgcolor, color, reflect, text, bold, border, inverted, nothing
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/color.hpp"  // for Color
#include "ftxui/util/ref.hpp"      // for Ref, ConstStringRef

namespace ftxui {

namespace {

Element DefaultTransform(EntryState params) {  // NOLINT
  auto element = text(params.label) | border;
  if (params.active) {
    element |= bold;
  }
  if (params.focused) {
    element |= inverted;
  }
  return element;
}

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
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 Ref<ButtonOption> option) {
  class Impl : public ComponentBase {
   public:
    Impl(ConstStringRef label,
         std::function<void()> on_click,
         Ref<ButtonOption> option)
        : label_(std::move(label)),
          on_click_(std::move(on_click)),
          option_(std::move(option)) {}

    // Component implementation:
    Element Render() override {
      const bool active = Active();
      const bool focused = Focused();
      const bool focused_or_hover = focused || mouse_hover_;

      float target = focused_or_hover ? 1.F : 0.F;  // NOLINT
      if (target != animator_background_.to()) {
        SetAnimationTarget(target);
      }

      auto focus_management = focused ? focus : active ? select : nothing;
      EntryState state = {
          *label_,
          false,
          active,
          focused_or_hover,
      };

      auto element =
          (option_->transform ? option_->transform : DefaultTransform)  //
          (state);
      return element | AnimatedColorStyle() | focus_management | reflect(box_);
    }

    Decorator AnimatedColorStyle() {
      Decorator style = nothing;
      if (option_->animated_colors.background.enabled) {
        style = style | bgcolor(Color::Interpolate(
                            animation_foreground_,  //
                            option_->animated_colors.background.inactive,
                            option_->animated_colors.background.active));
      }
      if (option_->animated_colors.foreground.enabled) {
        style = style | color(Color::Interpolate(
                            animation_foreground_,  //
                            option_->animated_colors.foreground.inactive,
                            option_->animated_colors.foreground.active));
      }
      return style;
    }

    void SetAnimationTarget(float target) {
      if (option_->animated_colors.foreground.enabled) {
        animator_foreground_ =
            animation::Animator(&animation_foreground_, target,
                                option_->animated_colors.foreground.duration,
                                option_->animated_colors.foreground.function);
      }
      if (option_->animated_colors.background.enabled) {
        animator_background_ =
            animation::Animator(&animation_background_, target,
                                option_->animated_colors.background.duration,
                                option_->animated_colors.background.function);
      }
    }

    void OnAnimation(animation::Params& p) override {
      animator_background_.OnAnimation(p);
      animator_foreground_.OnAnimation(p);
    }

    void OnClick() {
      on_click_();
      animation_background_ = 0.5F;  // NOLINT
      animation_foreground_ = 0.5F;  // NOLINT
      SetAnimationTarget(1.F);       // NOLINT
    }

    bool OnEvent(Event event) override {
      if (event.is_mouse()) {
        return OnMouseEvent(event);
      }

      if (event == Event::Return) {
        OnClick();
        return true;
      }
      return false;
    }

    bool OnMouseEvent(Event event) {
      mouse_hover_ =
          box_.Contain(event.mouse().x, event.mouse().y) && CaptureMouse(event);

      if (!mouse_hover_) {
        return false;
      }

      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Pressed) {
        TakeFocus();
        OnClick();
        return true;
      }

      return false;
    }

    bool Focusable() const final { return true; }

   private:
    ConstStringRef label_;
    std::function<void()> on_click_;
    bool mouse_hover_ = false;
    Box box_;
    Ref<ButtonOption> option_;
    float animation_background_ = 0;
    float animation_foreground_ = 0;
    animation::Animator animator_background_ =
        animation::Animator(&animation_background_);
    animation::Animator animator_foreground_ =
        animation::Animator(&animation_foreground_);
  };

  return Make<Impl>(std::move(label), std::move(on_click), std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
