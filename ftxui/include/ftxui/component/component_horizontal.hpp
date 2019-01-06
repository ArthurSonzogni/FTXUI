#ifndef FTXUI_COMPONENT_COMPONENT_HORIZONTAL_H_
#define FTXUI_COMPONENT_COMPONENT_HORIZONTAL_H_

#include "ftxui/component/component_direction.hpp"

namespace ftxui::component {

// A component where focus and events are automatically handled for you.
// It assumes its children are put in the horizontal direction.
class ComponentHorizontal : public ComponentDirection {
 public:
  ComponentHorizontal(Delegate*);
  bool HandleDirection(Event) override;
};

} // namespace ftxui::component

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_HORIZONTAL_H_ */
