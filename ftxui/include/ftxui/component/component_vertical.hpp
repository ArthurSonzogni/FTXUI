#ifndef FTXUI_COMPONENT_COMPONENT_VERTICAL_H_
#define FTXUI_COMPONENT_COMPONENT_VERTICAL_H_

#include "ftxui/component/component_direction.hpp"

namespace ftxui {
namespace component {

// A component where focus and events are automatically handled for you.
// It assumes its children are put in the vertical direction.
class ComponentVertical : public ComponentDirection {
 public:
  ComponentVertical(Delegate* delegate);
  bool HandleDirection(int key) override;
};

} // namespace component
} // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_VERTICAL_H_ */
