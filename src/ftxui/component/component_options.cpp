// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/component_options.hpp"

#include <ftxui/screen/color.hpp>  // for Color, Color::White, Color::Black, Color::GrayDark, Color::Blue, Color::GrayLight, Color::Red
#include <memory>                  // for shared_ptr
#include <utility>                 // for move
#include "ftxui/component/animation.hpp"  // for Function, Duration
#include "ftxui/dom/direction.hpp"
#include "ftxui/dom/elements.hpp"  // for operator|=, Element, text, bgcolor, inverted, bold, dim, operator|, color, borderEmpty, hbox, automerge, border, borderLight

namespace ftxui {

/// @brief A color option that can be animated.
/// @params _inactive The color when the component is inactive.
/// @params _active The color when the component is active.
/// @params _duration The duration of the animation.
/// @params _function The easing function of the animation.
/// @ingroup component
void AnimatedColorOption::Set(Color _inactive,
                              Color _active,
                              animation::Duration _duration,
                              animation::easing::Function _function) {
  enabled = true;
  inactive = _inactive;
  active = _active;
  duration = _duration;
  function = std::move(_function);
}

/// @brief Set how the underline should animate.
/// @param d The duration of the animation.
/// @param f The easing function of the animation.
/// @ingroup component
void UnderlineOption::SetAnimation(animation::Duration d,
                                   animation::easing::Function f) {
  SetAnimationDuration(d);
  SetAnimationFunction(std::move(f));
}

/// @brief Set how the underline should animate.
/// @param d The duration of the animation.
/// @ingroup component
void UnderlineOption::SetAnimationDuration(animation::Duration d) {
  leader_duration = d;
  follower_duration = d;
}

/// @brief Set how the underline should animate.
/// @param f The easing function of the animation.
/// @ingroup component
void UnderlineOption::SetAnimationFunction(animation::easing::Function f) {
  leader_function = f;
  follower_function = std::move(f);
}

/// @brief Set how the underline should animate.
/// This is useful to desynchronize the animation of the leader and the
/// follower.
/// @param f_leader The duration of the animation for the leader.
/// @param f_follower The duration of the animation for the follower.
/// @ingroup component
void UnderlineOption::SetAnimationFunction(
    animation::easing::Function f_leader,
    animation::easing::Function f_follower) {
  leader_function = std::move(f_leader);
  follower_function = std::move(f_follower);
}

/// @brief Standard options for an horizontal menu.
/// This can be useful to implement a tab bar.
/// @ingroup component
// static
MenuOption MenuOption::Horizontal() {
  MenuOption option;
  option.direction = Direction::Right;
  option.entries_option.transform = [](const EntryState& state) {
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

/// @brief Standard options for an animated horizontal menu.
/// This can be useful to implement a tab bar.
/// @ingroup component
// static
MenuOption MenuOption::HorizontalAnimated() {
  auto option = Horizontal();
  option.underline.enabled = true;
  return option;
}

/// @brief Standard options for a vertical menu.
/// This can be useful to implement a list of selectable items.
/// @ingroup component
// static
MenuOption MenuOption::Vertical() {
  MenuOption option;
  option.entries_option.transform = [](const EntryState& state) {
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

/// @brief Standard options for an animated vertical menu.
/// This can be useful to implement a list of selectable items.
/// @ingroup component
// static
MenuOption MenuOption::VerticalAnimated() {
  auto option = MenuOption::Vertical();
  option.entries_option.transform = [](const EntryState& state) {
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

/// @brief Standard options for a horitontal menu with some separator.
/// This can be useful to implement a tab bar.
/// @ingroup component
// static
MenuOption MenuOption::Toggle() {
  auto option = MenuOption::Horizontal();
  option.elements_infix = [] { return text("│") | automerge; };
  return option;
}

/// @brief Create a ButtonOption, highlighted using [] characters.
/// @ingroup component
// static
ButtonOption ButtonOption::Ascii() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    const std::string t = s.focused ? "[" + s.label + "]"  //
                                    : " " + s.label + " ";
    return text(t);
  };
  return option;
}

/// @brief Create a ButtonOption, inverted when focused.
/// @ingroup component
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

/// @brief Create a ButtonOption. The button is shown using a border, inverted
/// when focused. This is the current default.
/// @ingroup component
ButtonOption ButtonOption::Border() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    auto element = text(s.label) | border;
    if (s.active) {
      element |= bold;
    }
    if (s.focused) {
      element |= inverted;
    }
    return element;
  };
  return option;
}

/// @brief Create a ButtonOption, using animated colors.
/// @ingroup component
// static
ButtonOption ButtonOption::Animated() {
  return Animated(Color::Black, Color::GrayLight,  //
                  Color::GrayDark, Color::White);
}

/// @brief Create a ButtonOption, using animated colors.
/// @ingroup component
// static
ButtonOption ButtonOption::Animated(Color color) {
  return ButtonOption::Animated(
      Color::Interpolate(0.85F, color, Color::Black),   // NOLINT
      Color::Interpolate(0.10F, color, Color::White),   // NOLINT
      Color::Interpolate(0.10F, color, Color::Black),   // NOLINT
      Color::Interpolate(0.85F, color, Color::White));  // NOLINT
}

/// @brief Create a ButtonOption, using animated colors.
/// @ingroup component
// static
ButtonOption ButtonOption::Animated(Color background, Color foreground) {
  // NOLINTBEGIN
  return ButtonOption::Animated(
      /*bakground=*/background,
      /*foreground=*/foreground,
      /*background_active=*/foreground,
      /*foreground_active=*/background);
  // NOLINTEND
}

/// @brief Create a ButtonOption, using animated colors.
/// @ingroup component
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
/// @ingroup component
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
/// @ingroup component
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

/// @brief Standard options for the input component.
/// @ingroup component
// static
InputOption InputOption::Default() {
  InputOption option;
  option.transform = [](InputState state) {
    state.element |= color(Color::White);

    if (state.is_placeholder) {
      state.element |= dim;
    }

    if (state.focused) {
      state.element |= inverted;
    } else if (state.hovered) {
      state.element |= bgcolor(Color::GrayDark);
    }

    return state.element;
  };
  return option;
}

/// @brief Standard options for a more beautiful input component.
/// @ingroup component
// static
InputOption InputOption::Spacious() {
  InputOption option;
  option.transform = [](InputState state) {
    state.element |= borderEmpty;
    state.element |= color(Color::White);

    if (state.is_placeholder) {
      state.element |= dim;
    }

    if (state.focused) {
      state.element |= bgcolor(Color::Black);
    }

    if (state.hovered) {
      state.element |= bgcolor(Color::GrayDark);
    }

    return state.element;
  };
  return option;
}

}  // namespace ftxui
