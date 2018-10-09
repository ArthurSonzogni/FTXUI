#include "ftxui/component/component_horizontal.hpp"

namespace ftxui {
namespace component {

ComponentHorizontal::ComponentHorizontal(Delegate* delegate)
    : ComponentDirection(delegate) {}

bool ComponentHorizontal::HandleDirection(int key) {
  // Left pressed ?
  if (key == 68 || key == 'h') {
    Component* previous_sibling = active_child_->PreviousSibling();
    if (previous_sibling) {
      active_child_ = previous_sibling;
      return true;
    }
  }

  // Left pressed ?
  if (key == 67 || key == 'l') {
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
