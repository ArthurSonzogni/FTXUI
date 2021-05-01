#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include <algorithm>
#include <functional>

namespace ftxui {

Element RadioBox::Render() {
  std::vector<Element> elements;
  bool is_focused = Focused();
  boxes_.resize(entries.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    auto style =
        (focused == int(i) && is_focused) ? focused_style : unfocused_style;
    auto focus_management =
        (focused != int(i)) ? nothing : is_focused ? focus : select;

    const std::wstring& symbol = selected == int(i) ? checked : unchecked;
    elements.push_back(hbox(text(symbol), text(entries[i]) | style) |
                       focus_management | reflect(boxes_[i]));
  }
  return vbox(std::move(elements));
}

bool RadioBox::OnEvent(Event event) {
  if (!event.screen()->CaptureMouse())
    return false;
  if (event.is_mouse())
    return OnMouseEvent(event);

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

bool RadioBox::OnMouseEvent(Event event) {
  if (!event.screen()->CaptureMouse())
    return false;
  for (int i = 0; i < boxes_.size(); ++i) {
    if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
      continue;

    focused = i;
    TakeFocus();

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      cursor_position = i;
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
