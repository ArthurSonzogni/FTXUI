// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_REQUIREMENT_HPP
#define FTXUI_DOM_REQUIREMENT_HPP

#include "ftxui/screen/box.hpp"

namespace ftxui {

struct Requirement {
  // The required size to fully draw the element.
  int min_x = 0;
  int min_y = 0;

  // How much flexibility is given to the component.
  int flex_grow_x = 0;
  int flex_grow_y = 0;
  int flex_shrink_x = 0;
  int flex_shrink_y = 0;

  // Focus management to support the frame/focus/select element.
  enum Selection {
    NORMAL = 0,
    SELECTED = 1,
    FOCUSED = 2,
  };
  Selection selection = NORMAL;
  Box selected_box;
};

}  // namespace ftxui

#endif  // FTXUI_DOM_REQUIREMENT_HPP
