#ifndef FTXUI_COMPONENT_TOGGLE_H_
#define FTXUI_COMPONENT_TOGGLE_H_

#include "ftxui/component/component.hpp"
#include <functional>
#include <string>

namespace ftxui {

class Toggle : public Component {
 public:
  // Constructor.
  Toggle(Delegate*);

  // State.
  size_t activated = 0;
  std::vector<std::wstring> options = {L"On", L"Off"};

  // Callback.
  std::function<void()> on_change = [](){};

  // Component implementation.
  Element Render() override;
  bool OnEvent(Event) override;
};

}  // namespace ftxui::Component

#endif /* end of include guard: FTXUI_COMPONENT_TOGGLE_H_ */
