#ifndef FTXUI_COMPONENT_TOGGLE_H_
#define FTXUI_COMPONENT_TOGGLE_H_

#include "ftxui/component/component.hpp"
#include <functional>

namespace ftxui {
namespace component {

class Toggle : public Component {
 public:
  // Constructor.
  Toggle(Delegate*);

  // State.
  bool activated = true;
  std::wstring on = L"On";
  std::wstring off = L"Off";

  // Callback.
  std::function<void()> on_change = [](){};

  // Component implementation.
  dom::Element Render() override;
	bool Event(int key) override;
};

}  // namespace component
}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TOGGLE_H_ */
