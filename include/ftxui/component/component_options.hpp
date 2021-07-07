#ifndef FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP
#define FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP

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

};     // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP */
