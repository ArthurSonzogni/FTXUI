// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_BOX_HPP
#define FTXUI_SCREEN_BOX_HPP

namespace ftxui {

struct Box {
  int x_min = 0;
  int x_max = 0;
  int y_min = 0;
  int y_max = 0;
  bool isXInverted = false;  // false means the box box from x_min to x_max (in the case of a selection for example)
  bool isYInverted = false;  // false means the box box from y_min to y_max (in the case of a selection for example) 

  static auto Intersection(Box a, Box b) -> Box;
  static auto Union(Box a, Box b) -> Box;
  bool Contain(int x, int y) const;
  Box Clean() const;
  bool IsEmpty() const;
  bool operator==(const Box& other) const;
  bool operator!=(const Box& other) const;
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_BOX_HPP
