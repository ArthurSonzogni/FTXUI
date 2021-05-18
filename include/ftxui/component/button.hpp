#ifndef FTXUI_COMPONENT_BUTTON_HPP
#define FTXUI_COMPONENT_BUTTON_HPP

#include <functional>  // for function
#include <string>      // for wstring

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/screen/box.hpp"                // for Box
#include "ftxui/screen/string.hpp"             // for ConstStringRef

namespace ftxui {
struct Event;

/// @brief A button. An action is associated to the click event.
/// @ingroup dom
class ButtonBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static ButtonBase* From(Component);

  // Constructor.
  ButtonBase(ConstStringRef label, std::function<void()> on_click, bool border);
  ~ButtonBase() override = default;

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  ConstStringRef label_;
  std::function<void()> on_click_;
  bool border_;
  Box box_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_BUTTON_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
