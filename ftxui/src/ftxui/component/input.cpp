#include "ftxui/component/input.hpp"
#include "ftxui/util/string.hpp"

namespace ftxui {
namespace component {

Input::Input(Delegate* delegate): Component(delegate) {}
Input::~Input() {}

// Component implementation.
dom::Element Input::Render() {
  using namespace dom;
  bool is_focused = Focused();

  // Placeholder.
  if (content.size() == 0) {
    if (is_focused)
      return flex(inverted(dim(text(placeholder))));
    else
      return flex(dim(text(placeholder)));
  }

  // Not focused.
  if (!is_focused)
    return flex(text(content));

  std::wstring sub_content = content;
  size_t sub_cursor_position = cursor_position;

  std::wstring part_before_cursor = content.substr(0,cursor_position);
  std::wstring part_at_cursor = cursor_position < (int)content.size()
                                    ? content.substr(cursor_position, 1)
                                    : L" ";
  std::wstring part_after_cursor = cursor_position < (int)content.size() - 1
                                       ? content.substr(cursor_position + 1)
                                       : L"";
  return flex(inverted(hbox(             //
      text(part_before_cursor),          //
      underlined(text(part_at_cursor)),  //
      text(part_after_cursor)            //
      )));                               //
}
bool Input::OnEvent(Event event) {
  std::wstring c;

  // Backspace.
  if (event == Event::Backspace) {
    if (cursor_position == 0)
      return false;
    content.erase(cursor_position - 1, 1);
    cursor_position--;
    return true;
  }

  // Enter.
  if (event == Event::Return) {
    return true;
  }

  if (event == Event::ArrowLeft && cursor_position > 0) {
    cursor_position--;
    return true;
  }

  if (event == Event::ArrowRight && cursor_position < (int)content.size()) {
    cursor_position++;
    return true;
  }

  // Content
  constexpr char ESC = char(27);
  if (event.values[0] != ESC) {
    wchar_t v = (char)event.values[0];
    content.insert(cursor_position, 1, v);
    cursor_position++;
    return true;
  }
  return false;
}

}  // namespace component
}  // namespace ftxui
