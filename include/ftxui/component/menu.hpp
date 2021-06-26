#ifndef FTXUI_COMPONENT_MENU
#define FTXUI_COMPONENT_MENU

#include <functional>  // for function
#include <string>      // for wstring
#include <vector>      // for vector

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"  // for Element, Decorator, operator|, bold, inverted, nothing
#include "ftxui/screen/box.hpp"  // for Box

namespace ftxui {
struct Event;

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
template <class T = std::vector<std::wstring>*>
class MenuBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static MenuBase<T>* From(Component component);

  // Constructor.
  MenuBase(T entries, int* selected_);
  ~MenuBase() override = default;

  // State.
  int focused = 0;

  Decorator normal_style = nothing;
  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator selected_focused_style = focused_style | selected_style;

  // State update callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 protected:
  T const entries_;
  int* selected_ = 0;

  bool OnMouseEvent(Event);

  std::vector<Box> boxes_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_MENU */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
