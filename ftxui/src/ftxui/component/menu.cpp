#include "ftxui/component/menu.hpp"
#include <algorithm>
#include <iostream>

namespace ftxui {

Element Menu::Render() {
  std::vector<Element> elements;
  bool focused = Focused();
  for (size_t i = 0; i < entries.size(); ++i) {
    if (selected == i) {
      if (focused)
        elements.push_back(active_style(text(L"> " + entries[i])));
      else
        elements.push_back(selected_style(text(L"> " + entries[i])));
    }
    else {
      elements.push_back(normal_style(text(L"  " + entries[i])));
    }
  }
  return vbox(std::move(elements));
}

bool Menu::OnEvent(Event event) {
  if (!Focused())
    return false;

  size_t new_selected = selected;
  if (event == Event::ArrowUp || event == Event::Character('k'))
    new_selected--;
  if (event == Event::ArrowDown || event == Event::Character('j'))
    new_selected++;
  new_selected = std::max(size_t(0), std::min(entries.size()-size_t(1), new_selected));

  if (selected != new_selected) {
    selected = new_selected;
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
