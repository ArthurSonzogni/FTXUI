#ifndef FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP
#define FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP

#include <chrono>                         // for milliseconds
#include <ftxui/component/animation.hpp>  // for Duration, QuadraticInOut, Function
#include <ftxui/dom/elements.hpp>         // for Element
#include <ftxui/util/ref.hpp>             // for Ref
#include <functional>                     // for function
#include <optional>                       // for optional
#include <string>                         // for string

#include "ftxui/screen/color.hpp"  // for Color, Color::GrayDark, Color::White

namespace ftxui {

/// @brief arguments for |ButtonOption::transform|, |CheckboxOption::transform|,
/// |Radiobox::transform|, |MenuEntryOption::transform|,
/// |MenuOption::transform|.
struct EntryState {
  std::string label;  /// < The label to display.
  bool state;         /// < The state of the button/checkbox/radiobox
  bool active;        /// < Whether the entry is the active one.
  bool focused;       /// < Whether the entry is one focused by the user.
};

struct UnderlineOption {
  bool enabled = false;

  Color color_active = Color::White;
  Color color_inactive = Color::GrayDark;

  animation::easing::Function leader_function =
      animation::easing::QuadraticInOut;
  animation::easing::Function follower_function =
      animation::easing::QuadraticInOut;

  animation::Duration leader_duration = std::chrono::milliseconds(250);
  animation::Duration leader_delay = std::chrono::milliseconds(0);
  animation::Duration follower_duration = std::chrono::milliseconds(250);
  animation::Duration follower_delay = std::chrono::milliseconds(0);

  void SetAnimation(animation::Duration d, animation::easing::Function f);
  void SetAnimationDuration(animation::Duration d);
  void SetAnimationFunction(animation::easing::Function f);
  void SetAnimationFunction(animation::easing::Function f_leader,
                            animation::easing::Function f_follower);
};

/// @brief Option about a potentially animated color.
/// @ingroup component
struct AnimatedColorOption {
  void Set(
      Color inactive,
      Color active,
      animation::Duration duration = std::chrono::milliseconds(250),
      animation::easing::Function function = animation::easing::QuadraticInOut);

  bool enabled = false;
  Color inactive;
  Color active;
  animation::Duration duration = std::chrono::milliseconds(250);
  animation::easing::Function function = animation::easing::QuadraticInOut;
};

struct AnimatedColorsOption {
  AnimatedColorOption background;
  AnimatedColorOption foreground;
};

/// @brief Option for the MenuEntry component.
/// @ingroup component
struct MenuEntryOption {
  std::function<Element(const EntryState& state)> transform;
  AnimatedColorsOption animated_colors;
};

/// @brief Option for the Menu component.
/// @ingroup component
struct MenuOption {
  // Standard constructors:
  static MenuOption Horizontal();
  static MenuOption HorizontalAnimated();
  static MenuOption Vertical();
  static MenuOption VerticalAnimated();
  static MenuOption Toggle();

  // Style:
  UnderlineOption underline;
  MenuEntryOption entries;
  enum Direction { Up, Down, Left, Right };
  Direction direction = Down;
  std::function<Element()> elements_prefix;
  std::function<Element()> elements_infix;
  std::function<Element()> elements_postfix;

  // Observers:
  std::function<void()> on_change;  ///> Called when the seelcted entry changes.
  std::function<void()> on_enter;   ///> Called when the user presses enter.
  Ref<int> focused_entry = 0;
};

/// @brief Option for the AnimatedButton component.
/// @ingroup component
struct ButtonOption {
  // Standard constructors:
  static ButtonOption Ascii();
  static ButtonOption Simple();
  static ButtonOption Border();
  static ButtonOption Animated();
  static ButtonOption Animated(Color color);
  static ButtonOption Animated(Color background, Color foreground);
  static ButtonOption Animated(Color background,
                               Color foreground,
                               Color background_active,
                               Color foreground_active);

  // Style:
  std::function<Element(const EntryState&)> transform;
  AnimatedColorsOption animated_colors;
};

/// @brief Option for the Checkbox component.
/// @ingroup component
struct CheckboxOption {
  // Standard constructors:
  static CheckboxOption Simple();

  // Style:
  std::function<Element(const EntryState&)> transform;

  // Observer:
  /// Called when the user change the state.
  std::function<void()> on_change = [] {};
};

/// @brief Option for the Input component.
/// @ingroup component
struct InputOption {
  /// Called when the content changes.
  std::function<void()> on_change = [] {};
  /// Called when the user presses enter.
  std::function<void()> on_enter = [] {};

  /// Obscure the input content using '*'.
  Ref<bool> password = false;

  /// When set different from -1, this attributes is used to store the cursor
  /// position.
  Ref<int> cursor_position = -1;
};

/// @brief Option for the Radiobox component.
/// @ingroup component
struct RadioboxOption {
  // Standard constructors:
  static RadioboxOption Simple();

  // Style:
  std::function<Element(const EntryState&)> transform;

  // Observers:
  /// Called when the selected entry changes.
  std::function<void()> on_change = [] {};
  Ref<int> focused_entry = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
