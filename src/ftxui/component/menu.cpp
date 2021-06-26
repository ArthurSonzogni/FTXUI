#include <stddef.h>   // for size_t
#include <algorithm>  // for max, min
#include <memory>     // for shared_ptr, allocator_traits<>::value_type
#include <utility>    // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowUp, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/menu.hpp"
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released
#include "ftxui/component/screen_interactive.hpp"  // for Component

namespace ftxui {

/// @brief A list of text. The focused element is selected.
/// @param entries The list of entries in the menu.
/// @param selected The index of the currently selected element.
/// @ingroup component
/// @see MenuBase
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
/// auto menu = Menu(&entries, &selected);
/// screen.Loop(menu);
/// ```
///
/// ### Output
///
/// ```bash
/// > entry 1
///   entry 2
///   entry 3
/// ```
template <class T = std::vector<std::wstring>*>
Component Menu(T entries, int* selected) {
  return Make<MenuBase<T>>(entries, selected);
}

template <>
Component Menu<std::vector<std::wstring>*>(std::vector<std::wstring>* entries,
                                           int* selected) {
  return Make<MenuBase<std::vector<std::wstring>*>>(entries, selected);
}

template <>
Component Menu<Components>(Components entries, int* selected) {
  return Make<MenuBase<Components>>(entries, selected);
}

// static
template <class T>
MenuBase<T>* MenuBase<T>::From(Component component) {
  return static_cast<MenuBase<T>*>(component.get());
}

template MenuBase<std::vector<std::wstring>*>*
MenuBase<std::vector<std::wstring>*>::From(Component component);

template MenuBase<Components>* MenuBase<Components>::From(Component component);

template <class T>
MenuBase<T>::MenuBase(T entries, int* selected)
    : entries_(entries), selected_(selected) {}

template <>
MenuBase<Components>::MenuBase(Components entries, int* selected)
    : entries_(entries), selected_(selected) {
  auto cont = Container::Tab(entries, selected);
  cont->Add(Container::Vertical(entries));
  Add(std::move(cont));
}

template <>
Element MenuBase<std::vector<std::wstring>*>::Render() {
  Elements elements;
  bool is_menu_focused = Focused();
  boxes_.resize(entries_->size());
  for (size_t i = 0; i < entries_->size(); ++i) {
    bool is_focused = (focused == int(i)) && is_menu_focused;
    bool is_selected = (*selected_ == int(i));

    auto style = is_selected
                     ? (is_focused ? selected_focused_style : selected_style)
                     : (is_focused ? focused_style : normal_style);
    auto focus_management =
        !is_selected ? nothing : is_menu_focused ? focus : select;
    auto icon = is_selected ? L"> " : L"  ";
    elements.push_back(text(icon + entries_->at(i)) | style | focus_management |
                       reflect(boxes_[i]));
  }
  return vbox(std::move(elements));
}

template <>
Element MenuBase<Components>::Render() {
  Elements elements;
  boxes_.resize(entries_.size());
  bool is_menu_focused = Focused();
  for (size_t i = 0; i < entries_.size(); ++i) {
    if (entries_.at(i)->Active()) {
      *selected_ = i;
    }
    bool is_focused = (*selected_ == int(i)) && is_menu_focused;
    bool is_selected = (*selected_ == int(i)) && entries_.at(i)->Focused();

    auto style = is_selected
                     ? (is_focused ? selected_focused_style : selected_style)
                     : (is_focused ? focused_style : normal_style);
    auto focus_management =
        !is_selected ? nothing : is_menu_focused ? focus : select;
    auto icon = is_selected ? L"> " : L"  ";
    elements.push_back(window(text(icon), entries_.at(i)->Render()) | style |
                       focus_management | reflect(boxes_[i]));
  }
  return vbox(std::move(elements));
}

template <>
bool MenuBase<std::vector<std::wstring>*>::OnEvent(Event event) {
  if (!CaptureMouse(event))
    return false;
  if (event.is_mouse())
    return OnMouseEvent(event);

  if (!Focused())
    return false;

  int old_selected = *selected_;
  if (event == Event::ArrowUp || event == Event::Character('k'))
    (*selected_)--;
  if (event == Event::ArrowDown || event == Event::Character('j'))
    (*selected_)++;
  if (event == Event::Tab && entries_->size())
    *selected_ = (*selected_ + 1) % entries_->size();
  if (event == Event::TabReverse && entries_->size())
    *selected_ = (*selected_ + entries_->size() - 1) % entries_->size();

  *selected_ = std::max(0, std::min(int(entries_->size()) - 1, *selected_));

  if (*selected_ != old_selected) {
    focused = *selected_;
    on_change();
    return true;
  }

  if (event == Event::Return) {
    on_enter();
    return true;
  }

  return false;
}

template <>
bool MenuBase<Components>::OnEvent(Event event) {
  return ComponentBase::OnEvent(event);
}

template <class T>
bool MenuBase<T>::OnMouseEvent(Event event) {
  if (!CaptureMouse(event))
    return false;
  for (int i = 0; i < int(boxes_.size()); ++i) {
    if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
      continue;

    TakeFocus();
    focused = i;
    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Released) {
      if (*selected_ != i) {
        *selected_ = i;
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
