#ifndef FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP
#define FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP

#include <ftxui/dom/elements.hpp>

namespace ftxui {

struct MenuOption {
  Decorator normal_style = nothing;
  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator selected_focused_style = focused_style | selected_style;

  // State update callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};
};

struct ButtonOption {
  bool border = true;
};

struct CheckboxOption {
  std::wstring checked = L"▣ ";    /// Prefix for a "checked" state.
  std::wstring unchecked = L"☐ ";  /// Prefix for a "unchecked" state.

  Decorator focused_style = inverted;   /// Decorator used when focused.
  Decorator unfocused_style = nothing;  /// Decorator used when unfocused.

  /// Called when the user change the state.
  std::function<void()> on_change = []() {};
};

struct InputOption {
  std::function<void()> on_change = [] {};
  std::function<void()> on_enter = [] {};
};

struct RadioboxOption {
  std::wstring checked = L"◉ ";
  std::wstring unchecked = L"○ ";

  Decorator focused_style = inverted;
  Decorator unfocused_style = nothing;

  std::function<void()> on_change = []() {};
};

struct ToggleOption {
  Decorator normal_style = dim;
  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator selected_focused_style = focused_style | selected_style;

  // Callback.
  std::function<void()> on_change = []() {};
  std::function<void()> on_enter = []() {};
};

};  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP */
