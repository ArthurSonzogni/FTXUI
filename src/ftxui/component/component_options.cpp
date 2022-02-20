#include "ftxui/component/component_options.hpp"

#include <memory>  // for allocator, shared_ptr

#include "ftxui/component/animation.hpp"  // for Function, Duration
#include "ftxui/dom/elements.hpp"  // for Element, operator|, text, bold, dim, inverted, automerge

namespace ftxui {

void AnimatedColorOption::Set(Color a_inactive,
                              Color a_active,
                              animation::Duration a_duration,
                              animation::easing::Function a_function) {
  enabled = true;
  inactive = a_inactive;
  active = a_active;
  duration = a_duration;
  function = a_function;
}

void UnderlineOption::SetAnimation(animation::Duration d,
                                   animation::easing::Function f) {
  SetAnimationDuration(d);
  SetAnimationFunction(f);
}

void UnderlineOption::SetAnimationDuration(animation::Duration d) {
  leader_duration = d;
  follower_duration = d;
}

void UnderlineOption::SetAnimationFunction(animation::easing::Function f) {
  leader_function = f;
  follower_function = f;
}

void UnderlineOption::SetAnimationFunction(
    animation::easing::Function f_leader,
    animation::easing::Function f_follower) {
  leader_function = f_leader;
  follower_function = f_follower;
}

// static
MenuOption MenuOption::Horizontal() {
  MenuOption option;
  option.direction = Direction::Right;
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    Element e = text(label);
    if (focused)
      e = e | inverted;
    if (selected)
      e = e | bold;
    if (!focused && !selected)
      e = e | dim;
    return e;
  };
  option.elements_infix = [] { return text(" "); };

  return option;
}

// static
MenuOption MenuOption::HorizontalAnimated() {
  auto option = Horizontal();
  option.underline.enabled = true;
  return option;
}

// static
MenuOption MenuOption::Vertical() {
  MenuOption option;
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    if (selected)
      label = "> " + label;
    else
      label = "  " + label;

    Element e = text(label);
    if (focused)
      e = e | inverted;
    if (selected)
      e = e | bold;
    if (!focused && !selected)
      e = e | dim;
    return e;
  };
  return option;
}

// static
MenuOption MenuOption::VerticalAnimated() {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    Element e = text(label);
    if (focused)
      e = e | inverted;
    if (selected)
      e = e | bold;
    if (!focused && !selected)
      e = e | dim;
    return e;
  };
  option.underline.enabled = true;
  return option;
}

// static
MenuOption MenuOption::Toggle() {
  auto option = MenuOption::Horizontal();
  option.elements_infix = [] { return text("│") | automerge; };
  return option;
}

/// @brief Create a ButtonOption, highlighted using [] characters.
// static
ButtonOption Ascii() {
  ButtonOption option;
  option.transform = [](std::string label, bool focused) {
    label = focused ? "[" + label + "]"  //
                    : " " + label + " ";
    return text(label);
  };
  return option;
}

/// @brief Create a ButtonOption, inverted when focused.
// static
ButtonOption Simple() {
  ButtonOption option;
  option.transform = [](std::string label, bool focused) {
    auto element = text(label);
    if (focused)
      element |= inverted;
    return element;
  };
  return option;
}

/// @brief Create a ButtonOption, inverted when focused.
// static
ButtonOption ButtonOption::Simple() {
  ButtonOption option;
  option.transform = [](std::string label, bool focused) {
    auto element = text(label) | borderLight;
    if (focused)
      element |= inverted;
    return element;
  };
  return option;
}

/// @brief Create a ButtonOption, using animated colors.
// static
ButtonOption ButtonOption::Animated() {
  return Animated(Color::Black, Color::GrayLight,  //
                  Color::GrayDark, Color::White);
}

/// @brief Create a ButtonOption, using animated colors.
// static
ButtonOption ButtonOption::Animated(Color color) {
  return ButtonOption::Animated(Color::Interpolate(0.85f, color, Color::Black),
                                Color::Interpolate(0.10f, color, Color::White),
                                Color::Interpolate(0.10f, color, Color::Black),
                                Color::Interpolate(0.85f, color, Color::White));
}

/// @brief Create a ButtonOption, using animated colors.
// static
ButtonOption ButtonOption::Animated(Color background, Color foreground) {
  return ButtonOption::Animated(background, foreground, foreground, background);
}

/// @brief Create a ButtonOption, using animated colors.
// static
ButtonOption ButtonOption::Animated(Color background,
                                    Color foreground,
                                    Color background_focused,
                                    Color foreground_focused) {
  ButtonOption option;
  option.transform = [](std::string label, bool focused) {
    auto element = text(label) | borderEmpty;
    if (focused)
      element |= bold;
    return element;
  };
  option.animated_colors.foreground.Set(foreground, foreground_focused);
  option.animated_colors.background.Set(background, background_focused);
  return option;
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
