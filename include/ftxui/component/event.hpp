#ifndef FTXUI_COMPONENT_EVENT_HPP
#define FTXUI_COMPONENT_EVENT_HPP

#include <vector>
#include <array>

namespace ftxui {

struct Event{
 public:
  // --- Character ---
  static Event Character(int);

  // --- Arrow ---
  static Event ArrowLeft;
  static Event ArrowRight;
  static Event ArrowUp;
  static Event ArrowDown;
  
  // --- Other ---
  static Event Backspace;
  static Event Delete;
  static Event Return;
  static Event Escape;
  static Event F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12;

  // --- Custom ---
  static Event Custom;

  bool operator==(const Event& other) { return values == other.values; }

  // Internal representation.
  std::array<int, 5> values = {0, 0, 0, 0, 0};
  
};


} // namespace ftxui


#endif /* end of include guard: FTXUI_COMPONENT_EVENT_HPP */
