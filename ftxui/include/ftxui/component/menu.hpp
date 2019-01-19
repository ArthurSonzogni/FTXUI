#ifndef FTXUI_COMPONENT_MENU
#define FTXUI_COMPONENT_MENU

#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include <functional>

namespace ftxui {

class Menu : public Component {
 public:
  // Constructor.
  Menu() = default;
  ~Menu() override = default;

  // State.
  std::vector<std::wstring> entries = {};
  int selected = 0;

  Decorator focused_style = inverted;
  Decorator selected_style = bold;
  Decorator normal_style = nothing;

  // State update callback.
  std::function<void()> on_change = [](){};
  std::function<void()> on_enter = [](){};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;
};

}  // namespace ftxui::Component

#endif /* end of include guard: FTXUI_COMPONENT_MENU */
