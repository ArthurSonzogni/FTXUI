#include "ftxui/component/component_direction.hpp"

namespace ftxui {

ComponentDirection::ComponentDirection(Delegate* delegate)
    : Component(delegate), active_child_(nullptr) {}

bool ComponentDirection::OnEvent(Event event) {
  if (!Focused())
    return false;

  if (!active_child_)
    return false;

  if (active_child_->OnEvent(event))
    return true;

  return HandleDirection(event);

}

Component* ComponentDirection::GetActiveChild() {
  return active_child_;
}

void ComponentDirection::Focus(Component* child) {
  active_child_ = child;
}

} // namespace ftxui::Component
