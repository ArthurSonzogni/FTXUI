#include "ftxui/component/input.hpp"

#include <algorithm>

#include "ftxui/screen/string.hpp"

namespace ftxui {

// Component implementation.
Element Input::Render() {
  cursor_position = std::max(0, std::min<int>(content.size(), cursor_position));
  auto main_decorator = flex | size(HEIGHT, EQUAL, 1);
  bool is_focused = Focused();

  // Placeholder.
  if (content.size() == 0) {
    if (is_focused)
      return text(placeholder) | focus | dim | inverted | main_decorator |
             reflect(input_box_);
    else
      return text(placeholder) | dim | main_decorator | reflect(input_box_);
  }

  // Not focused.
  if (!is_focused)
    return text(content) | main_decorator | reflect(input_box_);

  std::wstring part_before_cursor = content.substr(0, cursor_position);
  std::wstring part_at_cursor = cursor_position < (int)content.size()
                                    ? content.substr(cursor_position, 1)
                                    : L" ";
  std::wstring part_after_cursor = cursor_position < (int)content.size() - 1
                                       ? content.substr(cursor_position + 1)
                                       : L"";
  auto focused = is_focused ? focus : select;

  // clang-format off
  return
    hbox(
      text(part_before_cursor),
      text(part_at_cursor) | underlined | focused | reflect(cursor_box_),
      text(part_after_cursor)
    ) | flex | inverted | frame | main_decorator | reflect(input_box_);
  // clang-format on
}

bool Input::OnEvent(Event event) {
  cursor_position = std::max(0, std::min<int>(content.size(), cursor_position));

  if (event.is_mouse())
    return OnMouseEvent(event);

  std::wstring c;

  // Backspace.
  if (event == Event::Backspace) {
    if (cursor_position == 0)
      return false;
    content.erase(cursor_position - 1, 1);
    cursor_position--;
    on_change();
    return true;
  }

  // Delete
  if (event == Event::Delete) {
    if (cursor_position == int(content.size()))
      return false;
    content.erase(cursor_position, 1);
    on_change();
    return true;
  }

  // Enter.
  if (event == Event::Return) {
    on_enter();
    return true;
  }

  if (event == Event::Custom) {
    return false;
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
  if (event.is_character()) {
    content.insert(cursor_position, 1, event.character());
    cursor_position++;
    on_change();
    return true;
  }
  return false;
}

bool Input::OnMouseEvent(Event event) {
  if (!input_box_.Contain(event.mouse().x, event.mouse().y))
    return false;

  TakeFocus();

  if (event.mouse().button == Mouse::Left &&
      event.mouse().motion == Mouse::Pressed) {
    int new_cursor_position =
        cursor_position + event.mouse().x - cursor_box_.x_min;
    new_cursor_position =
        std::max(0, std::min<int>(content.size(), new_cursor_position));
    if (cursor_position != new_cursor_position) {
      cursor_position = new_cursor_position;
      on_change();
    }
  }
  return true;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
