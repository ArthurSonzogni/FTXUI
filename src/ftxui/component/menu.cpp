#include <algorithm>   // for max, reverse
#include <chrono>      // for milliseconds
#include <functional>  // for function
#include <memory>  // for allocator, shared_ptr, allocator_traits<>::value_type, swap
#include <string>   // for char_traits, operator+, string, basic_string
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/animation.hpp"  // for Animator, Linear, Params (ptr only)
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"  // for Make, Menu, MenuEntry, Toggle
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption, MenuEntryOption, MenuOption::Direction, UnderlineOption, AnimatedColorOption, AnimatedColorsOption, MenuOption::Down, MenuOption::Left, MenuOption::Right, MenuOption::Up
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp, Event::End, Event::Home, Event::PageDown, Event::PageUp, Event::Return, Event::Tab, Event::TabReverse
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Released, Mouse::WheelDown, Mouse::WheelUp, Mouse::None
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, Decorator, nothing, Elements, bgcolor, color, hbox, separatorHSelector, separatorVSelector, vbox, xflex, yflex, text, bold, focus, inverted, select
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/color.hpp"  // for Color
#include "ftxui/screen/util.hpp"   // for clamp
#include "ftxui/util/ref.hpp"  // for Ref, ConstStringListRef, ConstStringRef

namespace ftxui {

namespace {

Element DefaultOptionTransform(const EntryState& state) {
  std::string label = (state.active ? "> " : "  ") + state.label;  // NOLINT
  Element e = text(label);
  if (state.focused) {
    e = e | inverted;
  }
  if (state.active) {
    e = e | bold;
  }
  return e;
}

bool IsInverted(MenuOption::Direction direction) {
  switch (direction) {
    case MenuOption::Direction::Up:
    case MenuOption::Direction::Left:
      return true;
    case MenuOption::Direction::Down:
    case MenuOption::Direction::Right:
      return false;
  }
  return false;  // NOT_REACHED()
}

bool IsHorizontal(MenuOption::Direction direction) {
  switch (direction) {
    case MenuOption::Direction::Left:
    case MenuOption::Direction::Right:
      return true;
    case MenuOption::Direction::Down:
    case MenuOption::Direction::Up:
      return false;
  }
  return false;  // NOT_REACHED()
}

}  // namespace

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
class MenuBase : public ComponentBase {
 public:
  MenuBase(ConstStringListRef entries, int* selected, Ref<MenuOption> option)
      : entries_(entries), selected_(selected), option_(std::move(option)) {}

  bool IsHorizontal() { return ftxui::IsHorizontal(option_->direction); }
  void OnChange() {
    if (option_->on_change) {
      option_->on_change();
    }
  }

  void OnEnter() {
    if (option_->on_enter) {
      option_->on_enter();
    }
  }

  void Clamp() {
    boxes_.resize(size());
    *selected_ = util::clamp(*selected_, 0, size() - 1);
    focused_entry() = util::clamp(focused_entry(), 0, size() - 1);
  }

  void OnAnimation(animation::Params& params) override {
    animator_first_.OnAnimation(params);
    animator_second_.OnAnimation(params);
    for (auto& animator : animator_background_) {
      animator.OnAnimation(params);
    }
    for (auto& animator : animator_foreground_) {
      animator.OnAnimation(params);
    }
  }

  Element Render() override {
    Clamp();
    UpdateAnimationTarget();

    Elements elements;
    bool is_menu_focused = Focused();
    if (option_->elements_prefix) {
      elements.push_back(option_->elements_prefix());
    }
    for (int i = 0; i < size(); ++i) {
      if (i != 0 && option_->elements_infix) {
        elements.push_back(option_->elements_infix());
      }
      bool is_focused = (focused_entry() == i) && is_menu_focused;
      bool is_selected = (*selected_ == i);

      auto focus_management = !is_selected      ? nothing
                              : is_menu_focused ? focus
                                                : nothing;
      EntryState state = {
          entries_[i],
          false,
          is_selected,
          is_focused,
      };

      Element element =
          (option_->entries.transform ? option_->entries.transform
                                      : DefaultOptionTransform)  //
          (state);
      elements.push_back(element | AnimatedColorStyle(i) | reflect(boxes_[i]) |
                         focus_management);
    }
    if (option_->elements_postfix) {
      elements.push_back(option_->elements_postfix());
    }

    if (IsInverted(option_->direction)) {
      std::reverse(elements.begin(), elements.end());
    }

    Element bar =
        IsHorizontal() ? hbox(std::move(elements)) : vbox(std::move(elements));

    if (!option_->underline.enabled) {
      return bar | reflect(box_);
    }

    if (IsHorizontal()) {
      return vbox({
                 bar | xflex,
                 separatorHSelector(first_, second_,  //
                                    option_->underline.color_active,
                                    option_->underline.color_inactive),
             }) |
             reflect(box_);
    } else {
      return hbox({
                 separatorVSelector(first_, second_,  //
                                    option_->underline.color_active,
                                    option_->underline.color_inactive),
                 bar | yflex,
             }) |
             reflect(box_);
    }
  }

  void OnUp() {
    switch (option_->direction) {
      case MenuOption::Direction::Up:
        (*selected_)++;
        break;
      case MenuOption::Direction::Down:
        (*selected_)--;
        break;
      case MenuOption::Direction::Left:
      case MenuOption::Direction::Right:
        break;
    }
  }

  void OnDown() {
    switch (option_->direction) {
      case MenuOption::Direction::Up:
        (*selected_)--;
        break;
      case MenuOption::Direction::Down:
        (*selected_)++;
        break;
      case MenuOption::Direction::Left:
      case MenuOption::Direction::Right:
        break;
    }
  }

  void OnLeft() {
    switch (option_->direction) {
      case MenuOption::Direction::Left:
        (*selected_)++;
        break;
      case MenuOption::Direction::Right:
        (*selected_)--;
        break;
      case MenuOption::Direction::Down:
      case MenuOption::Direction::Up:
        break;
    }
  }

  void OnRight() {
    switch (option_->direction) {
      case MenuOption::Direction::Left:
        (*selected_)--;
        break;
      case MenuOption::Direction::Right:
        (*selected_)++;
        break;
      case MenuOption::Direction::Down:
      case MenuOption::Direction::Up:
        break;
    }
  }

  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  bool OnEvent(Event event) override {
    Clamp();
    if (!CaptureMouse(event)) {
      return false;
    }

    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    if (Focused()) {
      int old_selected = *selected_;
      if (event == Event::ArrowUp || event == Event::Character('k')) {
        OnUp();
      }
      if (event == Event::ArrowDown || event == Event::Character('j')) {
        OnDown();
      }
      if (event == Event::ArrowLeft || event == Event::Character('h')) {
        OnLeft();
      }
      if (event == Event::ArrowRight || event == Event::Character('l')) {
        OnRight();
      }
      if (event == Event::PageUp) {
        (*selected_) -= box_.y_max - box_.y_min;
      }
      if (event == Event::PageDown) {
        (*selected_) += box_.y_max - box_.y_min;
      }
      if (event == Event::Home) {
        (*selected_) = 0;
      }
      if (event == Event::End) {
        (*selected_) = size() - 1;
      }
      if (event == Event::Tab && size()) {
        *selected_ = (*selected_ + 1) % size();
      }
      if (event == Event::TabReverse && size()) {
        *selected_ = (*selected_ + size() - 1) % size();
      }

      *selected_ = util::clamp(*selected_, 0, size() - 1);

      if (*selected_ != old_selected) {
        focused_entry() = *selected_;
        OnChange();
        return true;
      }
    }

    if (event == Event::Return) {
      OnEnter();
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
    if (!CaptureMouse(event)) {
      return false;
    }
    for (int i = 0; i < size(); ++i) {
      if (!boxes_[i].Contain(event.mouse().x, event.mouse().y)) {
        continue;
      }

      TakeFocus();
      focused_entry() = i;
      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Released) {
        if (*selected_ != i) {
          *selected_ = i;
          OnChange();
        }
        return true;
      }
    }
    return false;
  }

  bool OnMouseWheel(Event event) {
    if (!box_.Contain(event.mouse().x, event.mouse().y)) {
      return false;
    }
    int old_selected = *selected_;

    if (event.mouse().button == Mouse::WheelUp) {
      (*selected_)--;
    }
    if (event.mouse().button == Mouse::WheelDown) {
      (*selected_)++;
    }

    *selected_ = util::clamp(*selected_, 0, size() - 1);

    if (*selected_ != old_selected) {
      OnChange();
    }
    return true;
  }

  void UpdateAnimationTarget() {
    UpdateColorTarget();
    UpdateUnderlineTarget();
  }

  void UpdateColorTarget() {
    if (size() != (int)animation_background_.size()) {
      animation_background_.resize(size());
      animation_foreground_.resize(size());
      animator_background_.clear();
      animator_foreground_.clear();

      for (int i = 0; i < size(); ++i) {
        animation_background_[i] = 0.F;
        animation_foreground_[i] = 0.F;
        animator_background_.emplace_back(&animation_background_[i], 0.F,
                                          std::chrono::milliseconds(0),
                                          animation::easing::Linear);
        animator_foreground_.emplace_back(&animation_foreground_[i], 0.F,
                                          std::chrono::milliseconds(0),
                                          animation::easing::Linear);
      }
    }

    bool is_menu_focused = Focused();
    for (int i = 0; i < size(); ++i) {
      bool is_focused = (focused_entry() == i) && is_menu_focused;
      bool is_selected = (*selected_ == i);
      float target = is_selected ? 1.F : is_focused ? 0.5F : 0.F;  // NOLINT
      if (animator_background_[i].to() != target) {
        animator_background_[i] = animation::Animator(
            &animation_background_[i], target,
            option_->entries.animated_colors.background.duration,
            option_->entries.animated_colors.background.function);
        animator_foreground_[i] = animation::Animator(
            &animation_foreground_[i], target,
            option_->entries.animated_colors.foreground.duration,
            option_->entries.animated_colors.foreground.function);
      }
    }
  }

  Decorator AnimatedColorStyle(int i) {
    Decorator style = nothing;
    if (option_->entries.animated_colors.foreground.enabled) {
      style = style | color(Color::Interpolate(
                          animation_foreground_[i],
                          option_->entries.animated_colors.foreground.inactive,
                          option_->entries.animated_colors.foreground.active));
    }

    if (option_->entries.animated_colors.background.enabled) {
      style = style | bgcolor(Color::Interpolate(
                          animation_background_[i],
                          option_->entries.animated_colors.background.inactive,
                          option_->entries.animated_colors.background.active));
    }
    return style;
  }

  void UpdateUnderlineTarget() {
    if (!option_->underline.enabled) {
      return;
    }

    if (FirstTarget() == animator_first_.to() &&
        SecondTarget() == animator_second_.to()) {
      return;
    }

    if (FirstTarget() >= animator_first_.to()) {
      animator_first_ = animation::Animator(
          &first_, FirstTarget(), option_->underline.follower_duration,
          option_->underline.follower_function,
          option_->underline.follower_delay);

      animator_second_ = animation::Animator(
          &second_, SecondTarget(), option_->underline.leader_duration,
          option_->underline.leader_function, option_->underline.leader_delay);
    } else {
      animator_first_ = animation::Animator(
          &first_, FirstTarget(), option_->underline.leader_duration,
          option_->underline.leader_function, option_->underline.leader_delay);

      animator_second_ = animation::Animator(
          &second_, SecondTarget(), option_->underline.follower_duration,
          option_->underline.follower_function,
          option_->underline.follower_delay);
    }
  }

  bool Focusable() const final { return entries_.size(); }
  int& focused_entry() { return option_->focused_entry(); }
  int size() const { return int(entries_.size()); }
  float FirstTarget() {
    if (boxes_.empty()) {
      return 0.F;
    }
    int value = IsHorizontal() ? boxes_[*selected_].x_min - box_.x_min
                               : boxes_[*selected_].y_min - box_.y_min;
    return float(value);
  }
  float SecondTarget() {
    if (boxes_.empty()) {
      return 0.F;
    }
    int value = IsHorizontal() ? boxes_[*selected_].x_max - box_.x_min
                               : boxes_[*selected_].y_max - box_.y_min;
    return float(value);
  }

 protected:
  ConstStringListRef entries_;
  int* selected_ = nullptr;
  Ref<MenuOption> option_;

  std::vector<Box> boxes_;
  Box box_;

  float first_ = 0.F;
  float second_ = 0.F;
  animation::Animator animator_first_ = animation::Animator(&first_, 0.F);
  animation::Animator animator_second_ = animation::Animator(&second_, 0.F);

  std::vector<animation::Animator> animator_background_;
  std::vector<animation::Animator> animator_foreground_;
  std::vector<float> animation_background_;
  std::vector<float> animation_foreground_;
};

/// @brief A list of text. The focused element is selected.
/// @param entries The list of entries in the menu.
/// @param selected The index of the currently selected element.
/// @param option Additional optional parameters.
/// @ingroup component
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

/// @brief An horizontal list of elements. The user can navigate through them.
/// @param entries The list of selectable entries to display.
/// @param selected Reference the selected entry.
/// @param See also |Menu|.
/// @ingroup component
Component Toggle(ConstStringListRef entries, int* selected) {
  return Menu(entries, selected, MenuOption::Toggle());
}

/// @brief A specific menu entry. They can be put into a Container::Vertical to
/// form a menu.
/// @param label The text drawn representing this element.
/// @param option Additional optional parameters.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// int selected = 0;
/// auto menu = Container::Vertical({
///    MenuEntry("entry 1"),
///    MenuEntry("entry 2"),
///    MenuEntry("entry 3"),
/// }, &selected);
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
Component MenuEntry(ConstStringRef label, Ref<MenuEntryOption> option) {
  class Impl : public ComponentBase {
   public:
    Impl(ConstStringRef label, Ref<MenuEntryOption> option)
        : label_(std::move(label)), option_(std::move(option)) {}

   private:
    Element Render() override {
      bool focused = Focused();
      UpdateAnimationTarget();

      EntryState state = {
          *label_,
          false,
          hovered_,
          focused,
      };

      Element element =
          (option_->transform ? option_->transform : DefaultOptionTransform)  //
          (state);

      auto focus_management = focused ? select : nothing;
      return element | AnimatedColorStyle() | focus_management | reflect(box_);
    }

    void UpdateAnimationTarget() {
      bool focused = Focused();
      float target = focused ? 1.F : hovered_ ? 0.5F : 0.F;  // NOLINT
      if (target == animator_background_.to()) {
        return;
      }
      animator_background_ =
          animation::Animator(&animation_background_, target,
                              option_->animated_colors.background.duration,
                              option_->animated_colors.background.function);
      animator_foreground_ =
          animation::Animator(&animation_foreground_, target,
                              option_->animated_colors.foreground.duration,
                              option_->animated_colors.foreground.function);
    }

    Decorator AnimatedColorStyle() {
      Decorator style = nothing;
      if (option_->animated_colors.foreground.enabled) {
        style = style | color(Color::Interpolate(
                            animation_foreground_,
                            option_->animated_colors.foreground.inactive,
                            option_->animated_colors.foreground.active));
      }

      if (option_->animated_colors.background.enabled) {
        style = style | bgcolor(Color::Interpolate(
                            animation_background_,
                            option_->animated_colors.background.inactive,
                            option_->animated_colors.background.active));
      }
      return style;
    }

    bool Focusable() const override { return true; }
    bool OnEvent(Event event) override {
      if (!event.is_mouse()) {
        return false;
      }

      hovered_ = box_.Contain(event.mouse().x, event.mouse().y);

      if (!hovered_) {
        return false;
      }

      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Released) {
        TakeFocus();
        return true;
      }

      return false;
    }

    void OnAnimation(animation::Params& params) override {
      animator_background_.OnAnimation(params);
      animator_foreground_.OnAnimation(params);
    }

    ConstStringRef label_;
    Ref<MenuEntryOption> option_;
    Box box_;
    bool hovered_ = false;

    float animation_background_ = 0.F;
    float animation_foreground_ = 0.F;
    animation::Animator animator_background_ =
        animation::Animator(&animation_background_, 0.F);
    animation::Animator animator_foreground_ =
        animation::Animator(&animation_foreground_, 0.F);
  };

  return Make<Impl>(std::move(label), std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
