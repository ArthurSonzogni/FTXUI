#include "ftxui/component/component_direction.hpp"

namespace ftxui {
namespace component {

ComponentDirection::ComponentDirection(Delegate* delegate)
    : Component(delegate), active_child_(nullptr) {}

bool ComponentDirection::Event(int key) {
  if (!Focused())
    return false;

  if (!active_child_)
    return false;

  if (active_child_->Event(key))
    return true;

  return HandleDirection(key);
}

Component* ComponentDirection::GetActiveChild() {
  return active_child_;
}

void ComponentDirection::Focus(Component* child) {
  active_child_ = child;
}

} // namespace component
} // namespace ftxui
