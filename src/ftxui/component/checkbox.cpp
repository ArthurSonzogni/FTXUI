#include <functional>  // for function
#include <memory>      // for shared_ptr
#include <utility>     // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Component, Checkbox
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for CheckboxOption
#include "ftxui/component/event.hpp"              // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/dom/elements.hpp"  // for operator|, text, Element, hbox, reflect, focus, nothing, select
#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/util/ref.hpp"    // for Ref, ConstStringRef

namespace ftxui {

namespace {
class CheckboxBase : public ComponentBase {
 public:
  CheckboxBase(ConstStringRef label, bool* state, Ref<CheckboxOption> option)
      : label_(label), state_(state), option_(std::move(option)) {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    // Microsoft terminal do not use fonts able to render properly the default
    // radiobox glyph.
    if (option_->style_checked == "▣ ")
      option_->style_checked = "[X]";
    if (option_->style_unchecked == "☐ ")
      option_->style_unchecked = "[ ]";
#endif
  }

 private:
  // Component implementation.
  Element Render() override {
    bool is_focused = Focused();
    bool is_active = Active();
    auto style = (is_focused || hovered_) ? option_->style_selected_focused
                 : is_active              ? option_->style_selected
                                          : option_->style_normal;
    auto focus_management = is_focused ? focus : is_active ? select : nothing;
    return hbox({
               text(*state_ ? option_->style_checked
                            : option_->style_unchecked),
               text(*label_) | style | focus_management,
           }) |
           reflect(box_);
  }

  bool OnEvent(Event event) override {
    if (!CaptureMouse(event))
      return false;

    if (event.is_mouse())
      return OnMouseEvent(event);

    hovered_ = false;
    if (event == Event::Character(' ') || event == Event::Return) {
      *state_ = !*state_;
      option_->on_change();
      TakeFocus();
      return true;
    }
    return false;
  }

  bool OnMouseEvent(Event event) {
    hovered_ = box_.Contain(event.mouse().x, event.mouse().y);

    if (!CaptureMouse(event))
      return false;

    if (!hovered_)
      return false;

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      *state_ = !*state_;
      option_->on_change();
      return true;
    }

    return false;
  }

  bool Focusable() const final { return true; }

  ConstStringRef label_;
  bool* const state_;
  bool hovered_ = false;
  Ref<CheckboxOption> option_;
  Box box_;
};
}  // namespace

/// @brief Draw checkable element.
/// @param label The label of the checkbox.
/// @param checked Whether the checkbox is checked or not.
/// @param option Additional optional parameters.
/// @ingroup component
/// @see CheckboxBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::string label = "Make a sandwidth";
/// bool checked = false;
/// Component checkbox = Checkbox(&label, &checked);
/// screen.Loop(checkbox)
/// ```
///
/// ### Output
///
/// ```bash
/// ☐ Make a sandwitch
/// ```
Component Checkbox(ConstStringRef label,
                   bool* checked,
                   Ref<CheckboxOption> option) {
  return Make<CheckboxBase>(label, checked, std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
