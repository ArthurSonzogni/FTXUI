#include "ftxui/component/menu.hpp"

#include <algorithm>
#include <iostream>

namespace ftxui {

Element Menu::Render() {
  std::vector<Element> elements;
  bool is_focused = Focused();
  for (size_t i = 0; i < entries.size(); ++i) {
    auto style = (selected != int(i))
                     ? normal_style
                     : is_focused ? focused_style : selected_style;
    auto focused = (selected != int(i)) ? nothing : is_focused ? focus : select;
    auto icon = (selected != int(i)) ? L"  " : L"> ";
    elements.push_back(text(icon + entries[i]) | style | focused);
  }
  return vbox(std::move(elements));
}

bool Menu::OnEvent(Event event) {
  if (!Focused())
    return false;

  int old_selected = selected;
  if (event == Event::ArrowUp || event == Event::Character('k'))
    selected--;
  if (event == Event::ArrowDown || event == Event::Character('j'))
    selected++;
  if (event == Event::Tab && entries.size())
    selected = (selected + 1) % entries.size();
  if (event == Event::TabReverse && entries.size())
    selected = (selected + entries.size() - 1) % entries.size();

  selected = std::max(0, std::min(int(entries.size()) - 1, selected));

  if (selected != old_selected) {
    on_change();
    return true;
  }

  if (event == Event::Return) {
    on_enter();
    return true;
  }

  return false;
}

}  // namespace ftxui
