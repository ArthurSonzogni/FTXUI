#ifndef FTXUI_COMPONENT_RADIOBOX_HPP
#define FTXUI_COMPONENT_RADIOBOX_HPP

#include <functional>  // for function
#include <string>      // for wstring, allocator
#include <vector>      // for vector

#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"  // for Element, Decorator, inverted, nothing
#include "ftxui/screen/box.hpp"    // for Box

namespace ftxui {
struct Event;

/// @brief A list of selectable element. One and only one can be selected at
/// the same time.
/// @ingroup component
class RadioboxBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static RadioboxBase* From(Component component);

  // Constructor.
  RadioboxBase(const std::vector<std::wstring>* entries, int* selected);
  ~RadioboxBase() override = default;

  int focused = 0;

#if defined(_WIN32)
  std::wstring checked = L"(*) ";
  std::wstring unchecked = L"( ) ";
#else
  std::wstring checked = L"◉ ";
  std::wstring unchecked = L"○ ";
#endif

  Decorator focused_style = inverted;
  Decorator unfocused_style = nothing;

  // State update callback.
  std::function<void()> on_change = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  const std::vector<std::wstring>* const entries_;
  int* const selected_;

  bool OnMouseEvent(Event event);
  int cursor_position = 0;
  std::vector<Box> boxes_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_RADIOBOX_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
