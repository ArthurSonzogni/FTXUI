#include "ftxui/component/component_vertical.hpp"

namespace ftxui {
namespace component {

ComponentVertical::ComponentVertical(Delegate* delegate)
    : ComponentDirection(delegate) {}

bool ComponentVertical::HandleDirection(int key) {
  // Up pressed ?
  if (key == 65 || key == 'k') {
    Component* previous_sibling = active_child_->PreviousSibling();
    if (previous_sibling) {
      active_child_ = previous_sibling;
      return true;
    }
  }

  // Down pressed ?
  if (key == 66 || key == 'j') {
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
