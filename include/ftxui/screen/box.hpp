#ifndef FTXUI_SCREEN_BOX_HPP
#define FTXUI_SCREEN_BOX_HPP

namespace ftxui {

struct Box {
  int x_min = 0;
  int x_max = 0;
  int y_min = 0;
  int y_max = 0;

  static Box Intersection(Box a, Box b);
  bool Contain(int x, int y);
  bool operator==(const Box& other) const;
  bool operator!=(const Box& other) const;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_BOX_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
