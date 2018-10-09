#ifndef FTXUI_COMPONENT_COMPONENT_DIRECTION_H_
#define FTXUI_COMPONENT_COMPONENT_DIRECTION_H_

#include "ftxui/component/component.hpp"

namespace ftxui {
namespace component {

// A component where focus and events are automatically handled for you.
class ComponentDirection : public Component {
 public:
  ComponentDirection(Delegate* delegate);
  bool Event(int key) override;
  Component* GetActiveChild() override;

 protected:
  void Focus(Component* child);
  virtual bool HandleDirection(int key) = 0;
  Component* active_child_;
};

} // namespace component
} // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_DIRECTION_H_ */
