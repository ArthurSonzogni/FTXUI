#ifndef FTXUI_DOM_REQUIREMENT_HPP
#define FTXUI_DOM_REQUIREMENT_HPP

namespace ftxui::dom {

struct Requirement {
  // The required size to fully draw the element.
  struct { int x = 0; int y = 0; } min;

  // How much flexibility is given to the component.
  struct { int x = 0; int y = 0; } flex;
};

}; // namespace ftxui::dom

#endif /* end of include guard: FTXUI_REQUIREMENT_HPP */
