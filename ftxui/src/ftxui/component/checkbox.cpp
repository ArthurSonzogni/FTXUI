#include "ftxui/component/checkbox.hpp"
#include <functional>

namespace ftxui {

Element CheckBox::Render() {
  auto style = Focused() ? bold : nothing;
  return text((state ? checked : unchecked) + label) | style;
}

bool CheckBox::OnEvent(Event event) {
  if (event == Event::Character(' ') || event == Event::Return) {
    state = !state;
    on_change();
    return true;
  }
  return false;  
}

}  // namespace ftxui
