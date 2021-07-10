#include <stddef.h>    // for size_t
#include <algorithm>   // for max, min
#include <functional>  // for function
#include <memory>      // for shared_ptr, allocator_traits<>::value_type
#include <string>      // for operator+, wstring
#include <utility>     // for move
#include <vector>      // for vector, __alloc_traits<>::value_type

#include "ftxui/component/captured_mouse.hpp"     // for CapturedMouse
#include "ftxui/component/component.hpp"          // for Make, Menu
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowUp, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, text, vbox, Elements, focus, nothing, select
#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/util/ref.hpp"    // for Ref

namespace ftxui {

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
class MenuBase : public ComponentBase {
 public:
  MenuBase(const std::vector<std::wstring>* entries,
           int* selected,
           Ref<MenuOption> option)
      : entries_(entries), selected_(selected), option_(option) {}

  Element Render() {
    Elements elements;
    bool is_menu_focused = Focused();
    boxes_.resize(entries_->size());
    for (size_t i = 0; i < entries_->size(); ++i) {
      bool is_focused = (focused_entry() == int(i)) && is_menu_focused;
      bool is_selected = (*selected_ == int(i));

      auto style = is_selected ? (is_focused ? option_->style_selected_focused
                                             : option_->style_selected)
                               : (is_focused ? option_->style_focused
                                             : option_->style_normal);
      auto focus_management = !is_selected      ? nothing
                              : is_menu_focused ? focus
                                                : select;
      auto icon = is_selected ? L"> " : L"  ";
      elements.push_back(text(icon + entries_->at(i)) | style |
                         focus_management | reflect(boxes_[i]));
    }
    return vbox(std::move(elements));
  }

  bool OnEvent(Event event) {
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
      focused_entry() = *selected_;
      option_->on_change();
      return true;
    }

    if (event == Event::Return) {
      option_->on_enter();
      return true;
    }

    return false;
  }

  bool OnMouseEvent(Event event) {
    if (!CaptureMouse(event))
      return false;
    for (int i = 0; i < int(boxes_.size()); ++i) {
      if (!boxes_[i].Contain(event.mouse().x, event.mouse().y))
        continue;

      TakeFocus();
      focused_entry() = i;
      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Released) {
        if (*selected_ != i) {
          *selected_ = i;
          option_->on_change();
        }
        return true;
      }
    }
    return false;
  }

  int& focused_entry() { return option_->focused_entry(); }

 protected:
  const std::vector<std::wstring>* const entries_;
  int* selected_ = 0;
  Ref<MenuOption> option_;

  std::vector<Box> boxes_;
};

/// @brief A list of text. The focused element is selected.
/// @param entries The list of entries in the menu.
/// @param selected The index of the currently selected element.
/// @param option Additional optional parameters.
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
Component Menu(const std::vector<std::wstring>* entries,
               int* selected,
               Ref<MenuOption> option) {
  return Make<MenuBase>(entries, selected, std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
