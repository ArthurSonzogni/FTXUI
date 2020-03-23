#ifndef FTXUI_DOM_REQUIREMENT_HPP
#define FTXUI_DOM_REQUIREMENT_HPP

#include "ftxui/screen/box.hpp"

namespace ftxui {

struct Requirement {
  // The required size to fully draw the element.
  struct {
    int x = 0;
    int y = 0;
  } min;

  // How much flexibility is given to the component.
  struct {
    int x = 0;
    int y = 0;
  } flex;

  // Frame.
  enum Selection {
    NORMAL = 0,
    SELECTED = 1,
    FOCUSED = 2,
  } selection = NORMAL;
  Box selected_box;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_REQUIREMENT_HPP */
