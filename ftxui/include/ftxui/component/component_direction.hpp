#ifndef FTXUI_COMPONENT_COMPONENT_DIRECTION_H_
#define FTXUI_COMPONENT_COMPONENT_DIRECTION_H_

#include "ftxui/component/component.hpp"

namespace ftxui {

// A component where focus and events are automatically handled for you.
// Please use ComponentVertical or ComponentHorizontal.
class ComponentDirection : public Component {
 public:
  ComponentDirection(Delegate* delegate);
  bool OnEvent(Event) override;
  Component* GetActiveChild() override;

 protected:
  void Focus(Component* child);
  virtual bool HandleDirection(Event) = 0;
  Component* active_child_;
};

} // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_DIRECTION_H_ */
