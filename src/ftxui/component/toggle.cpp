#include <stddef.h>    // for size_t
#include <algorithm>   // for max, min
#include <functional>  // for function
#include <memory>      // for shared_ptr, allocator_traits<>::value_type
#include <utility>     // for move
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Toggle
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/component_options.hpp"  // for ToggleOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/dom/elements.hpp"  // for operator|, Element, Elements, hbox, reflect, separator, text, focus, nothing, select
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/util/ref.hpp"      // for ConstStringListRef, Ref

namespace ftxui {

namespace {

/// @brief An horizontal list of elements. The user can navigate through them.
/// @ingroup component
class ToggleBase : public ComponentBase {
 public:
  ToggleBase(ConstStringListRef entries,
             int* selected,
             Ref<ToggleOption> option)
      : entries_(entries), selected_(selected), option_(std::move(option)) {}

 private:
  Element Render() override {
    Elements children;
    bool is_toggle_focused = Focused();
    boxes_.resize(entries_.size());
    for (size_t i = 0; i < entries_.size(); ++i) {
      // Separator.
      if (i != 0)
        children.push_back(separator());

      bool is_focused = (focused_entry() == int(i)) && is_toggle_focused;
      bool is_selected = (*selected_ == int(i));

      auto style = is_selected ? (is_focused ? option_->style_selected_focused
                                             : option_->style_selected)
                               : (is_focused ? option_->style_focused
                                             : option_->style_normal);
      auto focus_management = !is_selected        ? nothing
                              : is_toggle_focused ? focus
                                                  : select;
      children.push_back(text(entries_[i]) | style | focus_management |
                         reflect(boxes_[i]));
    }
    return hbox(std::move(children));
  }

  bool OnEvent(Event event) override {
    if (event.is_mouse())
      return OnMouseEvent(event);

    int old_selected = *selected_;
    if (event == Event::ArrowLeft || event == Event::Character('h'))
      (*selected_)--;
    if (event == Event::ArrowRight || event == Event::Character('l'))
      (*selected_)++;
    if (event == Event::Tab && entries_.size())
      *selected_ = (*selected_ + 1) % entries_.size();
    if (event == Event::TabReverse && entries_.size())
      *selected_ = (*selected_ + entries_.size() - 1) % entries_.size();

    *selected_ = std::max(0, std::min(int(entries_.size()) - 1, *selected_));

    if (old_selected != *selected_) {
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
          event.mouse().motion == Mouse::Pressed) {
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

  bool Focusable() const final { return entries_.size(); }
  int& focused_entry() { return option_->focused_entry(); }

  ConstStringListRef entries_;
  int* selected_ = 0;

  std::vector<Box> boxes_;
  Ref<ToggleOption> option_;
};

}  // namespace

/// @brief An horizontal list of elements. The user can navigate through them.
/// @param entries The list of selectable entries to display.
/// @param selected Reference the selected entry.
/// @param option Additional optional parameters.
/// @ingroup component
Component Toggle(ConstStringListRef entries,
                 int* selected,
                 Ref<ToggleOption> option) {
  return Make<ToggleBase>(entries, selected, std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
