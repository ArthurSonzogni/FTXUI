#include <stddef.h>    // for size_t
#include <algorithm>   // for max, min
#include <functional>  // for function
#include <memory>      // for shared_ptr, allocator_traits<>::value_type
#include <utility>     // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowUp, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"  // for Component

namespace ftxui {

/// @brief A list of element, where only one can be selected.
/// @param entries The list of entries in the list.
/// @param selected The index of the currently selected element.
/// @ingroup component
/// @see RadioboxBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// std::vector<std::wstring> entries = {
///     L"entry 1",
///     L"entry 2",
///     L"entry 3",
/// };
/// int selected = 0;
/// auto menu = Radiobox(&entries, &selected);
/// screen.Loop(menu);
/// ```
///
/// ### Output
///
/// ```bash
/// ◉ entry 1
/// ○ entry 2
/// ○ entry 3
/// ```
Component Radiobox(const std::vector<std::wstring>* entries,
                   int* selected,
                   ConstRef<RadioboxOption> option) {
  return Make<RadioboxBase>(entries, selected, std::move(option));
}

// static
RadioboxBase* RadioboxBase::From(Component component) {
  return static_cast<RadioboxBase*>(component.get());
}

RadioboxBase::RadioboxBase(const std::vector<std::wstring>* entries,
                           int* selected,
                           ConstRef<RadioboxOption> option)
    : entries_(entries), selected_(selected), option_(std::move(option)) {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
  // Microsoft terminal do not use fonts able to render properly the default
  // radiobox glyph.
  if (option_->checked == L"◉ ")
    option_->checked = L"(*)";
  if (option_->unchecked == L"○ ")
    option_->unchecked = L"( )";
#endif
}

Element RadioboxBase::Render() {
  std::vector<Element> elements;
  bool is_focused = Focused();
  boxes_.resize(entries_->size());
  for (size_t i = 0; i < entries_->size(); ++i) {
    auto style = (focused == int(i) && is_focused) ? option_->style_focused
                                                   : option_->style_unfocused;
    auto focus_management = (focused != int(i)) ? nothing
                            : is_focused        ? focus
                                                : select;

    const std::wstring& symbol = *selected_ == int(i)
                                     ? option_->style_checked
                                     : option_->style_unchecked;
    elements.push_back(hbox(text(symbol), text(entries_->at(i)) | style) |
                       focus_management | reflect(boxes_[i]));
  }
  return vbox(std::move(elements));
}

bool RadioboxBase::OnEvent(Event event) {
  if (!CaptureMouse(event))
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
  if (event == Event::Tab && entries_->size())
    new_focused = (new_focused + 1) % entries_->size();
  if (event == Event::TabReverse && entries_->size())
    new_focused = (new_focused + entries_->size() - 1) % entries_->size();

  new_focused = std::max(0, std::min(int(entries_->size()) - 1, new_focused));

  if (focused != new_focused) {
    focused = new_focused;
    return true;
  }

  if (event == Event::Character(' ') || event == Event::Return) {
    *selected_ = focused;
    option_->on_change();
  }

  return false;
}

bool RadioboxBase::OnMouseEvent(Event event) {
  if (!CaptureMouse(event))
    return false;
  for (int i = 0; i < int(boxes_.size()); ++i) {
    if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
      continue;

    focused = i;
    TakeFocus();

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      cursor_position = i;
      TakeFocus();
      if (*selected_ != i) {
        *selected_ = i;
        option_->on_change();
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
