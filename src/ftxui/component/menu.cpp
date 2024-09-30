// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>                // for max, fill_n, reverse
#include <chrono>                   // for milliseconds
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <functional>               // for function
#include <string>                   // for operator+, string
#include <utility>                  // for move
#include <vector>                   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/animation.hpp"  // for Animator, Linear
#include "ftxui/component/component.hpp"  // for Make, Menu, MenuEntry, Toggle
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption, MenuEntryOption, UnderlineOption, AnimatedColorOption, AnimatedColorsOption, EntryState
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
  Element e = text(std::move(label));
  if (state.focused) {
    e = e | inverted;
  }
  if (state.active) {
    e = e | bold;
  }
  return e;
}

bool IsInverted(Direction direction) {
  switch (direction) {
    case Direction::Up:
    case Direction::Left:
      return true;
    case Direction::Down:
    case Direction::Right:
      return false;
  }
  return false;  // NOT_REACHED()
}

bool IsHorizontal(Direction direction) {
  switch (direction) {
    case Direction::Left:
    case Direction::Right:
      return true;
    case Direction::Down:
    case Direction::Up:
      return false;
  }
  return false;  // NOT_REACHED()
}

}  // namespace

/// @brief A list of items. The user can navigate through them.
/// @ingroup component
class MenuBase : public ComponentBase, public MenuOption {
 public:
  explicit MenuBase(const MenuOption& option) : MenuOption(option) {}

  bool IsHorizontal() { return ftxui::IsHorizontal(direction); }
  void OnChange() {
    if (on_change) {
      on_change();
    }
  }

  void OnEnter() {
    if (on_enter) {
      on_enter();
    }
  }

  void Clamp() {
    if (selected() != selected_previous_) {
      SelectedTakeFocus();
    }
    boxes_.resize(size());
    selected() = util::clamp(selected(), 0, size() - 1);
    selected_previous_ = util::clamp(selected_previous_, 0, size() - 1);
    selected_focus_ = util::clamp(selected_focus_, 0, size() - 1);
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
    const bool is_menu_focused = Focused();
    if (elements_prefix) {
      elements.push_back(elements_prefix());
    }
    elements.reserve(size());
    for (int i = 0; i < size(); ++i) {
      if (i != 0 && elements_infix) {
        elements.push_back(elements_infix());
      }
      const bool is_focused = (focused_entry() == i) && is_menu_focused;
      const bool is_selected = (selected() == i);

      const EntryState state = {
          entries[i], false, is_selected, is_focused, i,
      };

      auto focus_management = (selected_focus_ != i) ? nothing
                              : is_menu_focused      ? focus
                                                     : select;

      const Element element =
          (entries_option.transform ? entries_option.transform
                                    : DefaultOptionTransform)  //
          (state);
      elements.push_back(element | AnimatedColorStyle(i) | reflect(boxes_[i]) |
                         focus_management);
    }
    if (elements_postfix) {
      elements.push_back(elements_postfix());
    }

    if (IsInverted(direction)) {
      std::reverse(elements.begin(), elements.end());
    }

    const Element bar =
        IsHorizontal() ? hbox(std::move(elements)) : vbox(std::move(elements));

    if (!underline.enabled) {
      return bar | reflect(box_);
    }

    if (IsHorizontal()) {
      return vbox({
                 bar | xflex,
                 separatorHSelector(first_, second_,  //
                                    underline.color_active,
                                    underline.color_inactive),
             }) |
             reflect(box_);
    } else {
      return hbox({
                 separatorVSelector(first_, second_,  //
                                    underline.color_active,
                                    underline.color_inactive),
                 bar | yflex,
             }) |
             reflect(box_);
    }
  }

  void SelectedTakeFocus() {
    selected_previous_ = selected();
    selected_focus_ = selected();
  }

  void OnUp() {
    switch (direction) {
      case Direction::Up:
        selected()++;
        break;
      case Direction::Down:
        selected()--;
        break;
      case Direction::Left:
      case Direction::Right:
        break;
    }
  }

  void OnDown() {
    switch (direction) {
      case Direction::Up:
        selected()--;
        break;
      case Direction::Down:
        selected()++;
        break;
      case Direction::Left:
      case Direction::Right:
        break;
    }
  }

  void OnLeft() {
    switch (direction) {
      case Direction::Left:
        selected()++;
        break;
      case Direction::Right:
        selected()--;
        break;
      case Direction::Down:
      case Direction::Up:
        break;
    }
  }

  void OnRight() {
    switch (direction) {
      case Direction::Left:
        selected()--;
        break;
      case Direction::Right:
        selected()++;
        break;
      case Direction::Down:
      case Direction::Up:
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
      const int old_selected = selected();
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
        selected() -= box_.y_max - box_.y_min;
      }
      if (event == Event::PageDown) {
        selected() += box_.y_max - box_.y_min;
      }
      if (event == Event::Home) {
        selected() = 0;
      }
      if (event == Event::End) {
        selected() = size() - 1;
      }
      if (event == Event::Tab && size()) {
        selected() = (selected() + 1) % size();
      }
      if (event == Event::TabReverse && size()) {
        selected() = (selected() + size() - 1) % size();
      }

      selected() = util::clamp(selected(), 0, size() - 1);

      if (selected() != old_selected) {
        focused_entry() = selected();
        SelectedTakeFocus();
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
          event.mouse().motion == Mouse::Pressed) {
        if (selected() != i) {
          selected() = i;
          selected_previous_ = selected();
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
    const int old_selected = selected();

    if (event.mouse().button == Mouse::WheelUp) {
      selected()--;
    }
    if (event.mouse().button == Mouse::WheelDown) {
      selected()++;
    }

    selected() = util::clamp(selected(), 0, size() - 1);

    if (selected() != old_selected) {
      SelectedTakeFocus();
      OnChange();
    }
    return true;
  }

  void UpdateAnimationTarget() {
    UpdateColorTarget();
    UpdateUnderlineTarget();
  }

  void UpdateColorTarget() {
    if (size() != int(animation_background_.size())) {
      animation_background_.resize(size());
      animation_foreground_.resize(size());
      animator_background_.clear();
      animator_foreground_.clear();

      const int len = size();
      animator_background_.reserve(len);
      animator_foreground_.reserve(len);
      for (int i = 0; i < len; ++i) {
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

    const bool is_menu_focused = Focused();
    for (int i = 0; i < size(); ++i) {
      const bool is_focused = (focused_entry() == i) && is_menu_focused;
      const bool is_selected = (selected() == i);
      float target = is_selected ? 1.F : is_focused ? 0.5F : 0.F;  // NOLINT
      if (animator_background_[i].to() != target) {
        animator_background_[i] = animation::Animator(
            &animation_background_[i], target,
            entries_option.animated_colors.background.duration,
            entries_option.animated_colors.background.function);
        animator_foreground_[i] = animation::Animator(
            &animation_foreground_[i], target,
            entries_option.animated_colors.foreground.duration,
            entries_option.animated_colors.foreground.function);
      }
    }
  }

  Decorator AnimatedColorStyle(int i) {
    Decorator style = nothing;
    if (entries_option.animated_colors.foreground.enabled) {
      style = style | color(Color::Interpolate(
                          animation_foreground_[i],
                          entries_option.animated_colors.foreground.inactive,
                          entries_option.animated_colors.foreground.active));
    }

    if (entries_option.animated_colors.background.enabled) {
      style = style | bgcolor(Color::Interpolate(
                          animation_background_[i],
                          entries_option.animated_colors.background.inactive,
                          entries_option.animated_colors.background.active));
    }
    return style;
  }

  void UpdateUnderlineTarget() {
    if (!underline.enabled) {
      return;
    }

    if (FirstTarget() == animator_first_.to() &&
        SecondTarget() == animator_second_.to()) {
      return;
    }

    if (FirstTarget() >= animator_first_.to()) {
      animator_first_ = animation::Animator(
          &first_, FirstTarget(), underline.follower_duration,
          underline.follower_function, underline.follower_delay);

      animator_second_ = animation::Animator(
          &second_, SecondTarget(), underline.leader_duration,
          underline.leader_function, underline.leader_delay);
    } else {
      animator_first_ = animation::Animator(
          &first_, FirstTarget(), underline.leader_duration,
          underline.leader_function, underline.leader_delay);

      animator_second_ = animation::Animator(
          &second_, SecondTarget(), underline.follower_duration,
          underline.follower_function, underline.follower_delay);
    }
  }

  bool Focusable() const final { return entries.size(); }
  int size() const { return int(entries.size()); }
  float FirstTarget() {
    if (boxes_.empty()) {
      return 0.F;
    }
    const int value = IsHorizontal() ? boxes_[selected()].x_min - box_.x_min
                                     : boxes_[selected()].y_min - box_.y_min;
    return float(value);
  }
  float SecondTarget() {
    if (boxes_.empty()) {
      return 0.F;
    }
    const int value = IsHorizontal() ? boxes_[selected()].x_max - box_.x_min
                                     : boxes_[selected()].y_max - box_.y_min;
    return float(value);
  }

 protected:
  int selected_previous_ = selected();
  int selected_focus_ = selected();

  // Mouse click support:
  std::vector<Box> boxes_;
  Box box_;

  // Animation support:
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
/// @param option a structure containing all the paramters.
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
/// auto menu = Menu({
///   .entries = &entries,
///   .selected = &selected,
/// });
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
// NOLINTNEXTLINE
Component Menu(MenuOption option) {
  return Make<MenuBase>(std::move(option));
}

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
Component Menu(ConstStringListRef entries, int* selected, MenuOption option) {
  option.entries = std::move(entries);
  option.selected = selected;
  return Menu(option);
}

/// @brief An horizontal list of elements. The user can navigate through them.
/// @param entries The list of selectable entries to display.
/// @param selected Reference the selected entry.
/// See also |Menu|.
/// @ingroup component
Component Toggle(ConstStringListRef entries, int* selected) {
  return Menu(std::move(entries), selected, MenuOption::Toggle());
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
Component MenuEntry(ConstStringRef label, MenuEntryOption option) {
  option.label = std::move(label);
  return MenuEntry(std::move(option));
}

/// @brief A specific menu entry. They can be put into a Container::Vertical to
/// form a menu.
/// @param option The parameters.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// int selected = 0;
/// auto menu = Container::Vertical({
///    MenuEntry({.label = "entry 1"}),
///    MenuEntry({.label = "entry 2"}),
///    MenuEntry({.label = "entry 3"}),
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
Component MenuEntry(MenuEntryOption option) {
  class Impl : public ComponentBase, public MenuEntryOption {
   public:
    explicit Impl(MenuEntryOption option)
        : MenuEntryOption(std::move(option)) {}

   private:
    Element Render() override {
      const bool focused = Focused();
      UpdateAnimationTarget();

      const EntryState state{
          label(), false, hovered_, focused, Index(),
      };

      const Element element =
          (transform ? transform : DefaultOptionTransform)  //
          (state);

      auto focus_management = focused ? select : nothing;
      return element | AnimatedColorStyle() | focus_management | reflect(box_);
    }

    void UpdateAnimationTarget() {
      const bool focused = Focused();
      float target = focused ? 1.F : hovered_ ? 0.5F : 0.F;  // NOLINT
      if (target == animator_background_.to()) {
        return;
      }
      animator_background_ = animation::Animator(
          &animation_background_, target, animated_colors.background.duration,
          animated_colors.background.function);
      animator_foreground_ = animation::Animator(
          &animation_foreground_, target, animated_colors.foreground.duration,
          animated_colors.foreground.function);
    }

    Decorator AnimatedColorStyle() {
      Decorator style = nothing;
      if (animated_colors.foreground.enabled) {
        style = style |
                color(Color::Interpolate(animation_foreground_,
                                         animated_colors.foreground.inactive,
                                         animated_colors.foreground.active));
      }

      if (animated_colors.background.enabled) {
        style = style |
                bgcolor(Color::Interpolate(animation_background_,
                                           animated_colors.background.inactive,
                                           animated_colors.background.active));
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
          event.mouse().motion == Mouse::Pressed) {
        TakeFocus();
        return true;
      }

      return false;
    }

    void OnAnimation(animation::Params& params) override {
      animator_background_.OnAnimation(params);
      animator_foreground_.OnAnimation(params);
    }

    MenuEntryOption option_;
    Box box_;
    bool hovered_ = false;

    float animation_background_ = 0.F;
    float animation_foreground_ = 0.F;
    animation::Animator animator_background_ =
        animation::Animator(&animation_background_, 0.F);
    animation::Animator animator_foreground_ =
        animation::Animator(&animation_foreground_, 0.F);
  };

  return Make<Impl>(std::move(option));
}

}  // namespace ftxui
