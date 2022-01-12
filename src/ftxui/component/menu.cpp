#include <algorithm>   // for clamp, max
#include <functional>  // for function
#include <memory>      // for shared_ptr, allocator_traits<>::value_type
#include <string>      // for operator+, string
#include <utility>     // for move
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Menu, MenuEntry
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption, MenuEntryOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowUp, Event::End, Event::Home, Event::PageDown, Event::PageUp, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released, Mouse::WheelDown, Mouse::WheelUp, Mouse::None
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, text, nothing, select, vbox, Elements, focus
#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/screen/util.hpp"
#include "ftxui/util/ref.hpp"  // for Ref, ConstStringListRef, ConstStringRef

namespace ftxui {

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
class MenuBase : public ComponentBase {
 public:
  MenuBase(ConstStringListRef entries, int* selected, Ref<MenuOption> option)
      : entries_(entries), selected_(selected), option_(option) {}

  Element Render() override {
    Clamp();
    Elements elements;
    bool is_menu_focused = Focused();
    for (int i = 0; i < size(); ++i) {
      bool is_focused = (focused_entry() == i) && is_menu_focused;
      bool is_selected = (*selected_ == i);

      auto style = is_selected ? (is_focused ? option_->style_selected_focused
                                             : option_->style_selected)
                               : (is_focused ? option_->style_focused
                                             : option_->style_normal);
      auto focus_management = !is_selected      ? nothing
                              : is_menu_focused ? focus
                                                : select;
      auto icon = is_selected ? "> " : "  ";
      elements.push_back(text(icon + entries_[i]) | style | focus_management |
                         reflect(boxes_[i]));
    }
    return vbox(std::move(elements)) | reflect(box_);
  }

  bool OnEvent(Event event) override {
    Clamp();
    if (!CaptureMouse(event))
      return false;

    if (event.is_mouse())
      return OnMouseEvent(event);

    if (Focused()) {
      int old_selected = *selected_;
      if (event == Event::ArrowUp || event == Event::Character('k'))
        (*selected_)--;
      if (event == Event::ArrowDown || event == Event::Character('j'))
        (*selected_)++;
      if (event == Event::PageUp)
        (*selected_) -= box_.y_max - box_.y_min;
      if (event == Event::PageDown)
        (*selected_) += box_.y_max - box_.y_min;
      if (event == Event::Home)
        (*selected_) = 0;
      if (event == Event::End)
        (*selected_) = size() - 1;
      if (event == Event::Tab && size())
        *selected_ = (*selected_ + 1) % size();
      if (event == Event::TabReverse && size())
        *selected_ = (*selected_ + size() - 1) % size();

      *selected_ = util::clamp(*selected_, 0, size() - 1);

      if (*selected_ != old_selected) {
        focused_entry() = *selected_;
        option_->on_change();
        return true;
      }
    }

    if (event == Event::Return) {
      option_->on_enter();
      return true;
    }

    return false;
  }

  bool OnMouseEvent(Event event) {
    if (event.mouse().button == Mouse::WheelDown ||
        event.mouse().button == Mouse::WheelUp) {
      return OnMouseWheel(event);
    }

    if (event.mouse().button != Mouse::None &&
        event.mouse().button != Mouse::Left) {
      return false;
    }
    if (!CaptureMouse(event))
      return false;
    for (int i = 0; i < size(); ++i) {
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

  bool OnMouseWheel(Event event) {
    if (!box_.Contain(event.mouse().x, event.mouse().y))
      return false;
    int old_selected = *selected_;

    if (event.mouse().button == Mouse::WheelUp)
      (*selected_)--;
    if (event.mouse().button == Mouse::WheelDown)
      (*selected_)++;

    *selected_ = util::clamp(*selected_, 0, size() - 1);

    if (*selected_ != old_selected)
      option_->on_change();
    return true;
  }

  void Clamp() {
    boxes_.resize(size());
    *selected_ = util::clamp(*selected_, 0, size() - 1);
    focused_entry() = util::clamp(focused_entry(), 0, size() - 1);
  }

  bool Focusable() const final { return entries_.size(); }
  int& focused_entry() { return option_->focused_entry(); }
  int size() const { return entries_.size(); }

 protected:
  ConstStringListRef entries_;
  int* selected_ = 0;
  Ref<MenuOption> option_;

  std::vector<Box> boxes_;
  Box box_;
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
/// std::vector<std::string> entries = {
///     "entry 1",
///     "entry 2",
///     "entry 3",
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
Component Menu(ConstStringListRef entries,
               int* selected,
               Ref<MenuOption> option) {
  return Make<MenuBase>(entries, selected, std::move(option));
}

Component MenuEntry(ConstStringRef label, Ref<MenuEntryOption> option) {
  class Impl : public ComponentBase {
   public:
    Impl(ConstStringRef label, Ref<MenuEntryOption> option)
        : label_(std::move(label)), option_(std::move(option)) {}

   private:
    Element Render() override {
      bool focused = Focused();
      auto style =
          hovered_ ? (focused ? option_->style_selected_focused
                              : option_->style_selected)
                   : (focused ? option_->style_focused : option_->style_normal);
      auto focus_management = focused ? select : nothing;
      auto label = focused ? "> " + (*label_)  //
                           : "  " + (*label_);
      return text(label) | style | focus_management | reflect(box_);
    }
    bool Focusable() const override { return true; }
    bool OnEvent(Event event) override {
      if (!event.is_mouse())
        return false;

      hovered_ = box_.Contain(event.mouse().x, event.mouse().y);

      if (!hovered_)
        return false;

      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Released) {
        TakeFocus();
        return true;
      }

      return false;
    }
    ConstStringRef label_;
    Ref<MenuEntryOption> option_;
    Box box_;
    bool hovered_ = false;
  };

  return Make<Impl>(std::move(label), std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
