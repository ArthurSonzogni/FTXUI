#ifndef FTXUI_COMPONENT_TOGGLE_H_
#define FTXUI_COMPONENT_TOGGLE_H_

#include <functional>  // for function
#include <string>      // for wstring
#include <vector>      // for vector

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"  // for Element, Decorator, operator|, bold, dim, inverted
#include "ftxui/screen/box.hpp"  // for Box

namespace ftxui {
struct Event;

/// @brief An horizontal list of elements. The user can navigate through them.
/// @ingroup component
class ToggleBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static ToggleBase* From(Component component);

  // Constructor.
  ToggleBase(const std::vector<std::wstring>* entries, int* selected);
  ~ToggleBase() override = default;

  // State.
  int focused = 0;

  Decorator normal_style = dim;
  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator selected_focused_style = focused_style | selected_style;

  // Callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 protected:
  const std::vector<std::wstring>* const entries_;
  int* selected_ = 0;

  bool OnMouseEvent(Event event);
  std::vector<Box> boxes_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TOGGLE_H_ */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
