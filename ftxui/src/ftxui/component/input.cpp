#include "ftxui/component/input.hpp"
#include "ftxui/util/string.hpp"

namespace ftxui {
namespace component {

Input::Input(Delegate* delegate): Component(delegate) {}
Input::~Input() {}

// Component implementation.
dom::Element Input::Render() {
  bool is_place_ho
  std::wstring& displayed_text = content.size() ? content : placeholder;

  using namespace dom;
  if (Focused())
    return flex(inverted(text(displayed_text)));
  else
    return flex(text(displayed_text));
}
bool Input::OnEvent(Event event) {
  std::wstring c;

  // Backspace
  if (event == Event::Backspace) {
    if (content.size() != 0)
      content = content.substr(0, content.size()-1);
    return true;
  }

  // Enter
  if (event == Event::Return) {
    return true;
  }

  constexpr char ESC = char(27);
  if (event.values[0] != ESC) {
    content += event.values[0];
    return true;
  }

  return false;
}

}  // namespace component
}  // namespace ftxui
