#include "ftxui/component/menu.hpp"

#include <stddef.h>
#include <algorithm>
#include <memory>
#include <utility>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"

namespace ftxui {

Element Menu::Render() {
  Elements elements;
  bool is_menu_focused = Focused();
  boxes_.resize(entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    bool is_focused = (focused == int(i)) && is_menu_focused;
    bool is_selected = (selected == int(i));

    auto style = is_selected
                     ? (is_focused ? selected_focused_style : selected_style)
                     : (is_focused ? focused_style : normal_style);
    auto focus_management = !is_selected      ? nothing
                            : is_menu_focused ? focus
                                              : select;
    auto icon = is_selected ? L"> " : L"  ";
    elements.push_back(text(icon + entries[i]) | style | focus_management |
                       reflect(boxes_[i]));
  }
  return vbox(std::move(elements));
}

bool Menu::OnEvent(Event event) {
  if (!CaptureMouse(event))
    return false;
  if (event.is_mouse())
    return OnMouseEvent(event);

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
    focused = selected;
    on_change();
    return true;
  }

  if (event == Event::Return) {
    on_enter();
    return true;
  }

  return false;
}

bool Menu::OnMouseEvent(Event event) {
  if (!CaptureMouse(event))
    return false;
  for (int i = 0; i < boxes_.size(); ++i) {
    if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
      continue;

    TakeFocus();
    focused = i;
    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Released) {
      if (selected != i) {
        selected = i;
        on_change();
      }
      return true;
    }
  }
  return false;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
