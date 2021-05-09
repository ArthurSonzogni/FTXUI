#ifndef FTXUI_COMPONENT_INPUT_H_
#define FTXUI_COMPONENT_INPUT_H_

#include <functional>  // for function
#include <string>      // for wstring

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/screen/box.hpp"                // for Box

namespace ftxui {
struct Event;

/// @brief An input box. The user can type text into it.
/// @ingroup component.
class InputBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static InputBase* From(Component component);

  // Constructor.
  InputBase(std::wstring* content, const std::wstring* placeholder);
  ~InputBase() override = default;

  // State.
  int cursor_position = 0;

  // State update callback.
  std::function<void()> on_change = [] {};
  std::function<void()> on_enter = [] {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  std::wstring* const content_;
  const std::wstring* const placeholder_;

  bool OnMouseEvent(Event);
  Box input_box_;
  Box cursor_box_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_INPUT_H_ */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
