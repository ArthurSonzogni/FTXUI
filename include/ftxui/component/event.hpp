#ifndef FTXUI_COMPONENT_EVENT_HPP
#define FTXUI_COMPONENT_EVENT_HPP

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace ftxui {

// Documentation:
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
//
struct Event {
 public:
  // --- Constructor section ---------------------------------------------------
  static Event Character(char);
  static Event Character(wchar_t);

  static Event Character(const std::string&);
  static Event Special(const std::string&);

  static Event GetEvent(std::function<char()> getchar);

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
  static Event Tab;
  static Event TabReverse;
  static Event F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12;

  // --- Custom ---
  static Event Custom;

  //--- Method section ---------------------------------------------------------
  bool is_character() { return is_character_; }
  wchar_t character() { return character_; }
  const std::string& input() { return input_; }

  bool operator==(const Event& other) { return input_ == other.input_; }

  //--- State section ----------------------------------------------------------
 private:
  std::string input_;
  bool is_character_ = false;
  wchar_t character_ = U'?';
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_EVENT_HPP */
