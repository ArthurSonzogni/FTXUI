#ifndef FTXUI_DOM_FLEXBOX_HELPER_HPP
#define FTXUI_DOM_FLEXBOX_HELPER_HPP

#include <vector>
#include "ftxui/dom/flexbox_config.hpp"

namespace ftxui {
namespace flexbox_helper {

struct Block {
  // Input:
  int min_size_x = 0;
  int min_size_y = 0;
  int flex_grow_x = 0;
  int flex_grow_y = 0;
  int flex_shrink_x = 0;
  int flex_shrink_y = 0;

  // Output:
  int line;
  int line_position;
  int x = 0;
  int y = 0;
  int dim_x = 0;
  int dim_y = 0;
  bool overflow = false;
};

struct Global {
  std::vector<Block> blocks;
  FlexboxConfig config;
  int size_x;
  int size_y;
};

void Compute(Global& global);

}  // namespace flexbox_helper
}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_FLEXBOX_HELPER_HPP*/

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
