#ifndef FTXUI_COMPONENT_MENU
#define FTXUI_COMPONENT_MENU

#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include <functional>

namespace ftxui {
namespace component {

class Menu : public Component {
 public:
  // Constructor.
  Menu(Delegate*);

  // State.
  std::vector<std::wstring> entries = {};
  int selected = 0;

  dom::Decorator active_style = dom::inverted;
  dom::Decorator selected_style = dom::bold;
  dom::Decorator normal_style = dom::nothing;

  // State update callback.
  std::function<void()> on_change = [](){};
  std::function<void()> on_enter = [](){};

  // Component implementation.
  dom::Element Render() override;
	bool OnEvent(Event) override;
};

}  // namespace component
}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_MENU */
