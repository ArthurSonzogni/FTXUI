#include "ftxui/component/component_vertical.hpp"

namespace ftxui {
namespace component {

ComponentVertical::ComponentVertical(Delegate* delegate)
    : ComponentDirection(delegate) {}

bool ComponentVertical::HandleDirection(Event event) {
  // Up pressed ?
  if (event == Event::ArrowUp || event == Event::Character('k')) {
    Component* previous_sibling = active_child_->PreviousSibling();
    if (previous_sibling) {
      active_child_ = previous_sibling;
      return true;
    }
  }

  // Down pressed ?
  if (event == Event::ArrowDown || event == Event::Character('j')) {
    Component* next_sibling = active_child_->NextSibling();
    if (next_sibling) {
      active_child_ = next_sibling;
      return true;
    }
  }

  return false;
}

}  // namespace component
}  // namespace ftxui
