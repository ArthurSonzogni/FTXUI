#ifndef FTXUI_COMPONENT_TOGGLE_H_
#define FTXUI_COMPONENT_TOGGLE_H_

#include <functional>
#include <string>

#include "ftxui/component/component.hpp"

namespace ftxui {

/// @brief An horizontal list of elements. The user can navigate through them.
/// @ingroup component
class Toggle : public Component {
 public:
  // Constructor.
  ~Toggle() override = default;

  // State.
  int selected = 0;
  std::vector<std::wstring> entries = {L"On", L"Off"};

  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator normal_style = dim;

  // Callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TOGGLE_H_ */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
