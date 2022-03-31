#include "ftxui/component/component_options.hpp"

#include <memory>   // for shared_ptr
#include <utility>  // for move

#include "ftxui/component/animation.hpp"  // for Function, Duration
#include "ftxui/dom/elements.hpp"  // for operator|=, text, Element, bold, inverted, operator|, dim, hbox, automerge, borderEmpty, borderLight

namespace ftxui {

void AnimatedColorOption::Set(Color a_inactive,
                              Color a_active,
                              animation::Duration a_duration,
                              animation::easing::Function a_function) {
  enabled = true;
  inactive = a_inactive;
  active = a_active;
  duration = a_duration;
  function = std::move(a_function);
}

void UnderlineOption::SetAnimation(animation::Duration d,
                                   animation::easing::Function f) {
  SetAnimationDuration(d);
  SetAnimationFunction(std::move(f));
}

void UnderlineOption::SetAnimationDuration(animation::Duration d) {
  leader_duration = d;
  follower_duration = d;
}

void UnderlineOption::SetAnimationFunction(animation::easing::Function f) {
  leader_function = f;
  follower_function = std::move(f);
}

void UnderlineOption::SetAnimationFunction(
    animation::easing::Function f_leader,
    animation::easing::Function f_follower) {
  leader_function = std::move(f_leader);
  follower_function = std::move(f_follower);
}

// static
MenuOption MenuOption::Horizontal() {
  MenuOption option;
  option.direction = Direction::Right;
  option.entries.transform = [](const EntryState& state) {
    Element e = text(state.label);
    if (state.focused) {
      e |= inverted;
    }
    if (state.active) {
      e |= bold;
    }
    if (!state.focused && !state.active) {
      e |= dim;
    }
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
  option.entries.transform = [](const EntryState& state) {
    Element e = text((state.active ? "> " : "  ") + state.label);  // NOLINT
    if (state.focused) {
      e |= inverted;
    }
    if (state.active) {
      e |= bold;
    }
    if (!state.focused && !state.active) {
      e |= dim;
    }
    return e;
  };
  return option;
}

// static
MenuOption MenuOption::VerticalAnimated() {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](const EntryState& state) {
    Element e = text(state.label);
    if (state.focused) {
      e |= inverted;
    }
    if (state.active) {
      e |= bold;
    }
    if (!state.focused && !state.active) {
      e |= dim;
    }
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
ButtonOption ButtonOption::Ascii() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    std::string label = s.focused ? "[" + s.label + "]"  //
                                  : " " + s.label + " ";
    return text(label);
  };
  return option;
}

/// @brief Create a ButtonOption, inverted when focused.
// static
ButtonOption ButtonOption::Simple() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    auto element = text(s.label) | borderLight;
    if (s.focused) {
      element |= inverted;
    }
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
  return ButtonOption::Animated(
      Color::Interpolate(0.85F, color, Color::Black),   // NOLINT
      Color::Interpolate(0.10F, color, Color::White),   // NOLINT
      Color::Interpolate(0.10F, color, Color::Black),   // NOLINT
      Color::Interpolate(0.85F, color, Color::White));  // NOLINT
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
                                    Color background_active,
                                    Color foreground_active) {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    auto element = text(s.label) | borderEmpty;
    if (s.focused) {
      element |= bold;
    }
    return element;
  };
  option.animated_colors.foreground.Set(foreground, foreground_active);
  option.animated_colors.background.Set(background, background_active);
  return option;
}

/// @brief Option for standard Checkbox.
// static
CheckboxOption CheckboxOption::Simple() {
  auto option = CheckboxOption();
  option.transform = [](const EntryState& s) {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    // Microsoft terminal do not use fonts able to render properly the default
    // radiobox glyph.
    auto prefix = text(s.state ? "[X] " : "[ ] ");  // NOLINT
#else
    auto prefix = text(s.state ? "▣ " : "☐ ");  // NOLINT
#endif
    auto t = text(s.label);
    if (s.active) {
      t |= bold;
    }
    if (s.focused) {
      t |= inverted;
    }
    return hbox({prefix, t});
  };
  return option;
}

/// @brief Option for standard Radiobox
// static
RadioboxOption RadioboxOption::Simple() {
  auto option = RadioboxOption();
  option.transform = [](const EntryState& s) {
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    // Microsoft terminal do not use fonts able to render properly the default
    // radiobox glyph.
    auto prefix = text(s.state ? "(*) " : "( ) ");  // NOLINT
#else
    auto prefix = text(s.state ? "◉ " : "○ ");  // NOLINT
#endif
    auto t = text(s.label);
    if (s.active) {
      t |= bold;
    }
    if (s.focused) {
      t |= inverted;
    }
    return hbox({prefix, t});
  };
  return option;
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
