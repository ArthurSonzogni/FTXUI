#include "ftxui/component/component_horizontal.hpp"

namespace ftxui {
namespace component {

ComponentHorizontal::ComponentHorizontal(Delegate* delegate)
    : ComponentDirection(delegate) {}

bool ComponentHorizontal::HandleDirection(Event event) {
  // Left pressed ?
  if (event == Event::ArrowLeft || event == Event::Character('h')) {
    Component* previous_sibling = active_child_->PreviousSibling();
    if (previous_sibling) {
      active_child_ = previous_sibling;
      return true;
    }
  }

  // Left pressed ?
  if (event == Event::ArrowRight || event == Event::Character('l')) {
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
