#ifndef FTXUI_SCREEN_BOX_HPP
#define FTXUI_SCREEN_BOX_HPP

namespace ftxui {

struct Box {
  int x_min;
  int x_max;
  int y_min;
  int y_max;

  static Box Intersection(Box a, Box b);
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_BOX_HPP */
