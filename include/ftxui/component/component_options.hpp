#ifndef FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP
#define FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP

#include <chrono>                         // for milliseconds
#include <ftxui/component/animation.hpp>  // for Duration, QuadraticInOut, Function
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Left, Direction::Right, Direction::Down
#include <ftxui/dom/elements.hpp>  // for Element, separator
#include <ftxui/util/ref.hpp>      // for Ref, ConstRef, StringRef
#include <functional>              // for function
#include <optional>                // for optional
#include <string>                  // for string

#include "ftxui/component/component_base.hpp"  // for Component
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
  ConstStringRef label = "MenuEntry";
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

  ConstStringListRef entries;  ///> The list of entries.
  Ref<int> selected = 0;       ///> The index of the selected entry.

  // Style:
  UnderlineOption underline;
  MenuEntryOption entries_option;
  Direction direction = Direction::Down;
  std::function<Element()> elements_prefix;
  std::function<Element()> elements_infix;
  std::function<Element()> elements_postfix;

  // Observers:
  std::function<void()> on_change;  ///> Called when the selected entry changes.
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

  ConstStringRef label = "Button";
  std::function<void()> on_click = [] {};

  // Style:
  std::function<Element(const EntryState&)> transform;
  AnimatedColorsOption animated_colors;
};

/// @brief Option for the Checkbox component.
/// @ingroup component
struct CheckboxOption {
  // Standard constructors:
  static CheckboxOption Simple();

  ConstStringRef label = "Checkbox";

  Ref<bool> checked = false;

  // Style:
  std::function<Element(const EntryState&)> transform;

  // Observer:
  /// Called when the user change the state.
  std::function<void()> on_change = [] {};
};

/// @brief Used to define style for the Input component.
struct InputState {
  Element element;
  bool hovered;         /// < Whether the input is hovered by the mouse.
  bool focused;         /// < Whether the input is focused by the user.
  bool is_placeholder;  /// < Whether the input is empty and displaying the
                        /// < placeholder.
};

/// @brief Option for the Input component.
/// @ingroup component
struct InputOption {
  // A set of predefined styles:

  /// @brief Create the default input style:
  static InputOption Default();
  /// @brief A white on black style with high margins:
  static InputOption Spacious();

  /// The content of the input.
  StringRef content = "";

  /// The content of the input when it's empty.
  StringRef placeholder = "";

  // Style:
  std::function<Element(InputState)> transform;
  Ref<bool> password = false;  /// < Obscure the input content using '*'.
  Ref<bool> multiline = true;  /// < Whether the input can be multiline.

  /// Called when the content changes.
  std::function<void()> on_change = [] {};
  /// Called when the user presses enter.
  std::function<void()> on_enter = [] {};

  // The char position of the cursor:
  Ref<int> cursor_position = 0;
};

/// @brief Option for the Radiobox component.
/// @ingroup component
struct RadioboxOption {
  // Standard constructors:
  static RadioboxOption Simple();

  // Content:
  ConstStringListRef entries;
  Ref<int> selected = 0;

  // Style:
  std::function<Element(const EntryState&)> transform;

  // Observers:
  /// Called when the selected entry changes.
  std::function<void()> on_change = [] {};
  Ref<int> focused_entry = 0;
};

struct ResizableSplitOption {
  Component main;
  Component back;
  Ref<Direction> direction = Direction::Left;
  Ref<int> main_size =
      (direction() == Direction::Left || direction() == Direction::Right) ? 20
                                                                          : 10;
  std::function<Element()> separator_func = [] { return ::ftxui::separator(); };
};

// @brief Option for the `Slider` component.
// @ingroup component
template <typename T>
struct SliderOption {
  Ref<T> value;
  ConstRef<T> min = T(0);
  ConstRef<T> max = T(100);
  ConstRef<T> increment = (max() - min()) / 20;
  Direction direction = Direction::Right;
  Color color_active = Color::White;
  Color color_inactive = Color::GrayDark;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_OPTIONS_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
