#ifndef FTXUI_COMPONENT_CHECKBOX_HPP
#define FTXUI_COMPONENT_CHECKBOX_HPP

#include <functional>  // for function
#include <string>      // for allocator, wstring

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"   // for Element, Decorator, inverted, nothing
#include "ftxui/screen/box.hpp"     // for Box
#include "ftxui/screen/string.hpp"  // for ConstStringRef

namespace ftxui {
struct Event;

/// @brief A Checkbox. It can be checked or unchecked.Display an element on a
/// ftxui::Screen.
/// @ingroup dom
class CheckboxBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static CheckboxBase* From(Component component);

  // Constructor.
  CheckboxBase(ConstStringRef label,
               bool* state,
               ConstRef<CheckboxOption> option = {});
  ~CheckboxBase() override = default;

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  bool OnMouseEvent(Event event);

  ConstStringRef label_;
  bool* const state_;
  Box box_;
  ConstRef<CheckboxOption> option_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_CHECKBOX_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
