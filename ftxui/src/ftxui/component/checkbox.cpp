#include "ftxui/component/checkbox.hpp"
#include <functional>

namespace ftxui {

Element CheckBox::Render() {
  auto style = Focused() ? focused_style : unfocused_style;
  return hbox(text(state ? checked : unchecked), text(label) | style);
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
