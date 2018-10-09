#ifndef FTXUI_COMPONENT_EVENT
#define FTXUI_COMPONENT_EVENT

namespace ftxui {
namespace component {

struct Event{
  // --- Character ---
  static Event Character(char);

  // --- Arrow ---
  static Event Arrow_Left;
  static Event Arrow_Right;
  static Event Arrow_Up;
  static Event Arrow_Down;
  
  // --- Other ---
  static Event Backspace;
  static Event Delete;
  static Event Escape;
  static Event F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12;

  // Internal representation.
  int values [3];
  Event(int values[3]) : values(values);

};


} // namespace component
} // namespace ftxui


#endif /* end of include guard: FTXUI_COMPONENT_EVENT */
