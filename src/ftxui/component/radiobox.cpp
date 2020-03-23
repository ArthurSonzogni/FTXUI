#include "ftxui/component/radiobox.hpp"

#include <algorithm>
#include <functional>

namespace ftxui {

Element RadioBox::Render() {
  std::vector<Element> elements;
  bool is_focused = Focused();
  for (size_t i = 0; i < entries.size(); ++i) {
    auto style =
        (focused == int(i) && is_focused) ? focused_style : unfocused_style;
    auto focus_management =
        (focused != int(i)) ? nothing : is_focused ? focus : select;

    const std::wstring& symbol = selected == int(i) ? checked : unchecked;
    elements.push_back(hbox(text(symbol), text(entries[i]) | style) |
                       focus_management);
  }
  return vbox(std::move(elements));
}

bool RadioBox::OnEvent(Event event) {
  if (!Focused())
    return false;

  int new_focused = focused;
  if (event == Event::ArrowUp || event == Event::Character('k'))
    new_focused--;
  if (event == Event::ArrowDown || event == Event::Character('j'))
    new_focused++;
  if (event == Event::Tab && entries.size())
    new_focused = (new_focused + 1) % entries.size();
  if (event == Event::TabReverse && entries.size())
    new_focused = (new_focused + entries.size() - 1) % entries.size();

  new_focused = std::max(0, std::min(int(entries.size()) - 1, new_focused));

  if (focused != new_focused) {
    focused = new_focused;
    return true;
  }

  if (event == Event::Character(' ') || event == Event::Return) {
    selected = focused;
    on_change();
  }

  return false;
}

}  // namespace ftxui
