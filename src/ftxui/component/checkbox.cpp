// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <functional>  // for function
#include <utility>     // for move

#include "ftxui/component/component.hpp"       // for Make, Checkbox
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/component_options.hpp"  // for CheckboxOption, EntryState
#include "ftxui/component/event.hpp"              // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, focus, nothing, select
#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/util/ref.hpp"    // for Ref, ConstStringRef

namespace ftxui {

namespace {
class CheckboxBase : public ComponentBase, public CheckboxOption {
 public:
  explicit CheckboxBase(CheckboxOption option)
      : CheckboxOption(std::move(option)) {}

 private:
  // Component implementation.
  Element Render() override {
    const bool is_focused = Focused();
    const bool is_active = Active();
    auto focus_management = is_focused ? focus : is_active ? select : nothing;
    auto entry_state = EntryState{
        *label, *checked, is_active, is_focused || hovered_, -1,
    };
    auto element = (transform ? transform : CheckboxOption::Simple().transform)(
        entry_state);
    return element | focus_management | reflect(box_);
  }

  bool OnEvent(Event event) override {
    if (!CaptureMouse(event)) {
      return false;
    }

    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    hovered_ = false;
    if (event == Event::Character(' ') || event == Event::Return) {
      *checked = !*checked;
      on_change();
      TakeFocus();
      return true;
    }
    return false;
  }

  bool OnMouseEvent(Event event) {
    hovered_ = box_.Contain(event.mouse().x, event.mouse().y);

    if (!CaptureMouse(event)) {
      return false;
    }

    if (!hovered_) {
      return false;
    }

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      *checked = !*checked;
      on_change();
      TakeFocus();
      return true;
    }

    return false;
  }

  bool Focusable() const final { return true; }

  bool hovered_ = false;
  Box box_;
};
}  // namespace

/// @brief Draw checkable element.
/// @param option Additional optional parameters.
/// @ingroup component
/// @see CheckboxBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// CheckboxOption option;
/// option.label = "Make a sandwidth";
/// option.checked = false;
/// Component checkbox = Checkbox(option);
/// screen.Loop(checkbox)
/// ```
///
/// ### Output
///
/// ```bash
/// ☐ Make a sandwitch
/// ```
// NOLINTNEXTLINE
Component Checkbox(CheckboxOption option) {
  return Make<CheckboxBase>(std::move(option));
}

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
// NOLINTNEXTLINE
Component Checkbox(ConstStringRef label, bool* checked, CheckboxOption option) {
  option.label = std::move(label);
  option.checked = checked;
  return Make<CheckboxBase>(std::move(option));
}

}  // namespace ftxui
