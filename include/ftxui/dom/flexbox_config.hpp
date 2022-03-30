#ifndef FTXUI_DOM_FLEXBOX_CONFIG_HPP
#define FTXUI_DOM_FLEXBOX_CONFIG_HPP

/*
  This replicate the CSS flexbox model.
  See guide for documentation:
  https://css-tricks.com/snippets/css/a-guide-to-flexbox/
*/

namespace ftxui {

struct FlexboxConfig {
  /// This establishes the main-axis, thus defining the direction flex items are
  /// placed in the flex container. Flexbox is (aside wrapping) single-direction
  /// layout concept. Think of flex items as primarily laying out either in
  /// horizontal rows or vertical columns.
  enum class Direction {
    Row,            ///< Flex items are laid out in a row.
    RowInversed,    ///< Flex items are laid out in a row, but in reverse order.
    Column,         ///< Flex items are laid out in a column.
    ColumnInversed  ///< Flex items are laid out in a column, but in reverse
                    ///< order.
  };
  Direction direction = Direction::Row;

  /// By default, flex items will all try to fit onto one line. You can change
  /// that and allow the items to wrap as needed with this property.
  enum class Wrap {
    NoWrap,        ///< Flex items will all try to fit onto one line.
    Wrap,          ///< Flex items will wrap onto multiple lines.
    WrapInversed,  ///< Flex items will wrap onto multiple lines, but in reverse
                   ///< order.
  };
  Wrap wrap = Wrap::Wrap;

  /// This defines the alignment along the main axis. It helps distribute extra
  /// free space leftover when either all the flex items on a line are
  /// inflexible, or are flexible but have reached their maximum size. It also
  /// exerts some control over the alignment of items when they overflow the
  /// line.
  enum class JustifyContent {
    /// Items are aligned to the start of flexbox's direction.
    FlexStart,
    /// Items are aligned to the end of flexbox's direction.
    FlexEnd,
    /// Items are centered along the line.
    Center,
    /// Items are stretched to fill the line.
    Stretch,
    /// Items are evenly distributed in the line; first item is on the start
    // line, last item on the end line
    SpaceBetween,
    /// Items are evenly distributed in the line with equal space around them.
    /// Note that visually the spaces aren’t equal, since all the items have
    /// equal space on both sides. The first item will have one unit of space
    /// against the container edge, but two units of space between the next item
    /// because that next item has its own spacing that applies.
    SpaceAround,
    /// Items are distributed so that the spacing between any two items (and the
    /// space to the edges) is equal.
    SpaceEvenly,
  };
  JustifyContent justify_content = JustifyContent::FlexStart;

  /// This defines the default behavior for how flex items are laid out along
  /// the cross axis on the current line. Think of it as the justify-content
  /// version for the cross-axis (perpendicular to the main-axis).
  enum class AlignItems {
    FlexStart,  ///< items are placed at the start of the cross axis.
    FlexEnd,    ///< items are placed at the end of the cross axis.
    Center,     ///< items are centered along the cross axis.
    Stretch,    ///< items are stretched to fill the cross axis.
  };
  AlignItems align_items = AlignItems::FlexStart;

  // This aligns a flex container’s lines within when there is extra space in
  // the cross-axis, similar to how justify-content aligns individual items
  // within the main-axis.
  enum class AlignContent {
    FlexStart,     ///< items are placed at the start of the cross axis.
    FlexEnd,       ///< items are placed at the end of the cross axis.
    Center,        ///< items are centered along the cross axis.
    Stretch,       ///< items are stretched to fill the cross axis.
    SpaceBetween,  ///< items are evenly distributed in the cross axis.
    SpaceAround,   ///< tems evenly distributed with equal space around each
                   ///< line.
    SpaceEvenly,  ///< items are evenly distributed in the cross axis with equal
                  ///< space around them.
  };
  AlignContent align_content = AlignContent::FlexStart;

  int gap_x = 0;
  int gap_y = 0;

  // Constructor pattern. For chained use like:
  // ```
  // FlexboxConfig()
  //    .Set(FlexboxConfig::Direction::Row)
  //    .Set(FlexboxConfig::Wrap::Wrap);
  // ```
  FlexboxConfig& Set(FlexboxConfig::Direction);
  FlexboxConfig& Set(FlexboxConfig::Wrap);
  FlexboxConfig& Set(FlexboxConfig::JustifyContent);
  FlexboxConfig& Set(FlexboxConfig::AlignItems);
  FlexboxConfig& Set(FlexboxConfig::AlignContent);
  FlexboxConfig& SetGap(int gap_x, int gap_y);
};

}  // namespace ftxui

#endif  // FTXUI_DOM_FLEXBOX_CONFIG_HPP

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
