#ifndef FTXUI_REQUIREMENT_HPP
#define FTXUI_REQUIREMENT_HPP

namespace ftxui {

struct Requirement {
  // Minimal dimensions.
  struct {
    int x = 0;
    int y = 0;
  } min;

  // Maximal dimensions.
  struct {
    int x = -1;
    int y = -1;
  } max;

  // Flex.
  struct {
    int x = 0;
    int y = 0;
  } flex;
};

};  // namespace ftxui

#endif /* end of include guard: FTXUI_REQUIREMENT_HPP */
