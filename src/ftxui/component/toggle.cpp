#include "ftxui/component/toggle.hpp"

#include <algorithm>

namespace ftxui {

Element Toggle::Render() {
  Elements children;
  bool is_toggle_focused = Focused();
  boxes_.resize(entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    // Separator.
    if (i != 0)
      children.push_back(separator());

    bool is_focused = (focused == int(i)) && is_toggle_focused;
    bool is_selected = (selected == int(i));

    auto style = is_selected
                     ? (is_focused ? selected_focused_style : selected_style)
                     : (is_focused ? focused_style : normal_style);
    auto focus_management = !is_selected        ? nothing
                            : is_toggle_focused ? focus
                                                : select;
    children.push_back(text(entries[i]) | style | focus_management |
                       reflect(boxes_[i]));
  }
  return hbox(std::move(children));
}

bool Toggle::OnEvent(Event event) {
  if (event.is_mouse())
    return OnMouseEvent(event);

  int old_selected = selected;
  if (event == Event::ArrowLeft || event == Event::Character('h'))
    selected--;
  if (event == Event::ArrowRight || event == Event::Character('l'))
    selected++;
  if (event == Event::Tab && entries.size())
    selected = (selected + 1) % entries.size();
  if (event == Event::TabReverse && entries.size())
    selected = (selected + entries.size() - 1) % entries.size();

  selected = std::max(0, std::min(int(entries.size()) - 1, selected));

  if (old_selected != selected) {
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

bool Toggle::OnMouseEvent(Event event) {
  for (int i = 0; i < boxes_.size(); ++i) {
    if (!boxes_[i].Contain(event.mouse_x(), event.mouse_y()))
      continue;

    TakeFocus();
    focused = i;
    if (event.is_mouse_left_down()) {
      TakeFocus();
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
