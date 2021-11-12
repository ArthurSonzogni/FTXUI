#ifndef FTXUI_DOM_BOX_HELPER_HPP
#define FTXUI_DOM_BOX_HELPER_HPP

#include <vector>

namespace ftxui {
namespace box_helper {

struct Element {
  // Input:
  int min_size = 0;
  int flex_grow = 0;
  int flex_shrink = 0;

  // Output;
  int size = 0;
};

void Compute(std::vector<Element>* elements, int target_size);

}  // namespace box_helper
}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_BOX_HELPER_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.line.
