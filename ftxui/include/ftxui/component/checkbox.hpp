#ifndef FTXUI_COMPONENT_CHECKBOX_HPP
#define FTXUI_COMPONENT_CHECKBOX_HPP

#include "ftxui/component/component.hpp"
#include <functional>

namespace ftxui {

class CheckBox : public Component {
 public:
  // Constructor.
  CheckBox() = default;
  ~CheckBox() override = default;

  bool state = false;
  std::wstring label = L"label";

  std::wstring checked = L"[X] ";
  std::wstring unchecked = L"[ ] ";

  // State update callback.
  std::function<void()> on_change = [](){};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;

 private:
  int cursor_position = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_CHECKBOX_HPP */
