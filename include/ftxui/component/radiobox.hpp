#ifndef FTXUI_COMPONENT_RADIOBOX_HPP
#define FTXUI_COMPONENT_RADIOBOX_HPP

#include <functional>

#include "ftxui/component/component.hpp"

namespace ftxui {

class RadioBox : public Component {
 public:
  // Constructor.
  RadioBox() = default;
  ~RadioBox() override = default;

  int selected = 0;
  int focused = 0;
  std::vector<std::wstring> entries;

  std::wstring checked = L"◉ ";
  std::wstring unchecked = L"○ ";

  Decorator focused_style = inverted;
  Decorator unfocused_style = nothing;

  // State update callback.
  std::function<void()> on_change = []() {};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  int cursor_position = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_RADIOBOX_HPP */
