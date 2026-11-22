// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include <string_view>
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/direction.hpp"
#include "ftxui/dom/flexbox_config.hpp"
#include "ftxui/dom/linear_gradient.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/box.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/screen/terminal.hpp"
#include "ftxui/util/export.hpp"
#include "ftxui/util/ref.hpp"

namespace ftxui {
class Node;
using Element = std::shared_ptr<Node>;
using Elements = std::vector<Element>;
using Decorator = std::function<Element(Element)>;
using GraphFunction = std::function<std::vector<int>(int, int)>;

/// @brief BorderStyle is an enumeration that represents the different styles
/// of borders that can be applied to elements in the terminal UI.
///
/// BorderStyle is an enumeration that represents the different styles of
/// borders that can be applied to elements in the terminal UI.
/// It is used to define the visual appearance of borders around elements,
/// such as windows, frames, or separators.
/// @ingroup dom
enum BorderStyle : uint8_t {
  LIGHT,
  DASHED,
  HEAVY,
  DOUBLE,
  ROUNDED,
  EMPTY,
};

// Pipe elements into decorators together.
// For instance the next lines are equivalent:
// -> text("ftxui") | bold | underlined
// -> underlined(bold(text("FTXUI")))

/// Apply a decorator to an element.
FTXUI_EXPORT(DOM) Element operator|(Element, Decorator);
/// Apply a decorator to an element in place.
FTXUI_EXPORT(DOM) Element& operator|=(Element&, Decorator);
/// Apply a decorator to every element in a list.
FTXUI_EXPORT(DOM) Elements operator|(Elements, Decorator);
/// Compose two decorators into one.
FTXUI_EXPORT(DOM) Decorator operator|(Decorator, Decorator);

// --- Widget ---

/// Display a piece of UTF-8 encoded text.
FTXUI_EXPORT(DOM) Element text(std::string_view text);
/// Display a piece of UTF-8 encoded text vertically.
FTXUI_EXPORT(DOM) Element vtext(std::string_view text);
/// Draw a vertical or horizontal separation line in between two elements.
FTXUI_EXPORT(DOM) Element separator();
/// Draw a light separation line.
FTXUI_EXPORT(DOM) Element separatorLight();
/// Draw a dashed separation line.
FTXUI_EXPORT(DOM) Element separatorDashed();
/// Draw a heavy separation line.
FTXUI_EXPORT(DOM) Element separatorHeavy();
/// Draw a double separation line.
FTXUI_EXPORT(DOM) Element separatorDouble();
/// Draw an empty separation line (blank space).
FTXUI_EXPORT(DOM) Element separatorEmpty();
/// Draw a separation line with the given border style.
FTXUI_EXPORT(DOM) Element separatorStyled(BorderStyle);
/// Draw a separation line using a custom cell.
FTXUI_EXPORT(DOM) Element separator(Cell);
/// Draw a separation line using a custom character.
FTXUI_EXPORT(DOM) Element separatorCharacter(std::string_view);
/// Draw a horizontal separator highlighting the [left, right] relative range.
FTXUI_EXPORT(DOM)
Element separatorHSelector(float left,
                           float right,
                           Color unselected_color,
                           Color selected_color);
/// Draw a vertical separator highlighting the [up, down] relative range.
FTXUI_EXPORT(DOM)
Element separatorVSelector(float up,
                           float down,
                           Color unselected_color,
                           Color selected_color);
/// Draw a horizontal progress bar (progress in [0, 1]).
FTXUI_EXPORT(DOM) Element gauge(float progress);
/// Draw a progress bar filling from right to left (progress in [0, 1]).
FTXUI_EXPORT(DOM) Element gaugeLeft(float progress);
/// Draw a progress bar filling from left to right (progress in [0, 1]).
FTXUI_EXPORT(DOM) Element gaugeRight(float progress);
/// Draw a vertical progress bar filling bottom-to-top (progress in [0, 1]).
FTXUI_EXPORT(DOM) Element gaugeUp(float progress);
/// Draw a vertical progress bar filling top-to-bottom (progress in [0, 1]).
FTXUI_EXPORT(DOM) Element gaugeDown(float progress);
/// Draw a progress bar filling in the given direction (progress in [0, 1]).
FTXUI_EXPORT(DOM) Element gaugeDirection(float progress, Direction direction);
/// Draw a progress bar using custom shading glyphs (empty to full).
FTXUI_EXPORT(DOM)
Element gaugeCharset(float progress,
                     std::vector<std::string> charset,
                     Direction direction = Direction::Right);
/// Draw a box border around the element.
FTXUI_EXPORT(DOM) Element border(Element);
/// Draw a light box border around the element.
FTXUI_EXPORT(DOM) Element borderLight(Element);
/// Draw a dashed box border around the element.
FTXUI_EXPORT(DOM) Element borderDashed(Element);
/// Draw a heavy box border around the element.
FTXUI_EXPORT(DOM) Element borderHeavy(Element);
/// Draw a double box border around the element.
FTXUI_EXPORT(DOM) Element borderDouble(Element);
/// Draw a rounded box border around the element.
FTXUI_EXPORT(DOM) Element borderRounded(Element);
/// Draw an empty border (1-cell padding) around the element.
FTXUI_EXPORT(DOM) Element borderEmpty(Element);
/// Decorator drawing a border with the given style.
FTXUI_EXPORT(DOM) Decorator borderStyled(BorderStyle);
/// Decorator drawing a colored border with the given style.
FTXUI_EXPORT(DOM) Decorator borderStyled(BorderStyle, Color);
/// Decorator drawing a colored border.
FTXUI_EXPORT(DOM) Decorator borderStyled(Color);
/// Decorator drawing a border using a custom cell.
FTXUI_EXPORT(DOM) Decorator borderWith(const Cell&);
/// Draw a bordered window with a title and content.
FTXUI_EXPORT(DOM)
Element window(Element title, Element content, BorderStyle border = ROUNDED);
/// Draw an animated spinner frame from the given charset.
FTXUI_EXPORT(DOM) Element spinner(int charset_index, size_t image_index);
/// Display word-wrapped text (alias for paragraphAlignLeft).
FTXUI_EXPORT(DOM) Element paragraph(std::string_view text);
/// Display word-wrapped text aligned to the left.
FTXUI_EXPORT(DOM) Element paragraphAlignLeft(std::string_view text);
/// Display word-wrapped text aligned to the right.
FTXUI_EXPORT(DOM) Element paragraphAlignRight(std::string_view text);
/// Display word-wrapped text centered horizontally.
FTXUI_EXPORT(DOM) Element paragraphAlignCenter(std::string_view text);
/// Display word-wrapped text justified across the available width.
FTXUI_EXPORT(DOM) Element paragraphAlignJustify(std::string_view text);
/// Draw a plot from a function returning Y values for a given (width, height).
FTXUI_EXPORT(DOM) Element graph(GraphFunction);
/// Return an empty element taking no space.
FTXUI_EXPORT(DOM) Element emptyElement();
/// Render a Canvas (or reference to a Canvas) as an element.
FTXUI_EXPORT(DOM) Element canvas(ConstRef<Canvas>);
/// Render a Canvas of the given minimum size using a drawing callback.
FTXUI_EXPORT(DOM)
Element canvas(int width, int height, std::function<void(Canvas&)>);
/// Render a Canvas sized to the available area using a drawing callback.
FTXUI_EXPORT(DOM) Element canvas(std::function<void(Canvas&)>);

// --- Decorator ---

/// Render text in bold.
FTXUI_EXPORT(DOM) Element bold(Element);
/// Render text dimmed (reduced intensity).
FTXUI_EXPORT(DOM) Element dim(Element);
/// Render text in italics.
FTXUI_EXPORT(DOM) Element italic(Element);
/// Swap foreground and background colors.
FTXUI_EXPORT(DOM) Element inverted(Element);
/// Render text with a single underline.
FTXUI_EXPORT(DOM) Element underlined(Element);
/// Render text with a double underline.
FTXUI_EXPORT(DOM) Element underlinedDouble(Element);
/// Render blinking text.
FTXUI_EXPORT(DOM) Element blink(Element);
/// Render text with a strikethrough line.
FTXUI_EXPORT(DOM) Element strikethrough(Element);
/// Decorator setting the foreground color.
FTXUI_EXPORT(DOM) Decorator color(Color);
/// Decorator setting the background color.
FTXUI_EXPORT(DOM) Decorator bgcolor(Color);
/// Decorator setting a foreground linear gradient.
FTXUI_EXPORT(DOM) Decorator color(const LinearGradient&);
/// Decorator setting a background linear gradient.
FTXUI_EXPORT(DOM) Decorator bgcolor(const LinearGradient&);
/// Set the foreground color of an element.
FTXUI_EXPORT(DOM) Element color(Color, Element);
/// Set the background color of an element.
FTXUI_EXPORT(DOM) Element bgcolor(Color, Element);
/// Set a foreground linear gradient on an element.
FTXUI_EXPORT(DOM) Element color(const LinearGradient&, Element);
/// Set a background linear gradient on an element.
FTXUI_EXPORT(DOM) Element bgcolor(const LinearGradient&, Element);
/// Focus a specific (x, y) cell coordinate inside a scrollable frame.
FTXUI_EXPORT(DOM) Decorator focusPosition(int x, int y);
/// Focus a relative (x, y) position in [0, 1] inside a scrollable frame.
FTXUI_EXPORT(DOM) Decorator focusPositionRelative(float x, float y);
/// Automatically merge adjacent box-drawing characters (borders/separators).
FTXUI_EXPORT(DOM) Element automerge(Element child);
/// Decorator turning an element into a clickable OSC-8 terminal hyperlink.
FTXUI_EXPORT(DOM) Decorator hyperlink(std::string_view link);
/// Turn an element into a clickable OSC-8 terminal hyperlink.
FTXUI_EXPORT(DOM) Element hyperlink(std::string_view link, Element child);
/// Reset text-selection styling to the default inverted style.
FTXUI_EXPORT(DOM) Element selectionStyleReset(Element);
/// Decorator setting the foreground color of selected text.
FTXUI_EXPORT(DOM) Decorator selectionColor(Color foreground);
/// Decorator setting the background color of selected text.
FTXUI_EXPORT(DOM) Decorator selectionBackgroundColor(Color foreground);
/// Decorator setting the foreground color of selected text.
FTXUI_EXPORT(DOM) Decorator selectionForegroundColor(Color foreground);
/// Decorator customizing the cell style of selected text.
FTXUI_EXPORT(DOM) Decorator selectionStyle(std::function<void(Cell&)> style);

// --- Layout ---

/// Lay out elements side by side horizontally.
FTXUI_EXPORT(DOM) Element hbox(Elements);
/// Lay out elements stacked vertically.
FTXUI_EXPORT(DOM) Element vbox(Elements);
/// Stack elements on top of each other (depth box).
FTXUI_EXPORT(DOM) Element dbox(Elements);
/// Lay out elements using a CSS-like flexbox model.
FTXUI_EXPORT(DOM)
Element flexbox(Elements, FlexboxConfig config = FlexboxConfig());
/// Lay out a 2D grid of elements with aligned rows and columns.
FTXUI_EXPORT(DOM) Element gridbox(std::vector<Elements> lines);
/// Lay out elements in wrapping horizontal rows (flexbox helper).
FTXUI_EXPORT(DOM) Element hflow(Elements);
/// Lay out elements in wrapping vertical columns (flexbox helper).
FTXUI_EXPORT(DOM) Element vflow(Elements);

// --- Flexibility ---
// Define how to share the remaining space when not all of it is used inside a
// container.

/// Allow the element to expand or shrink on both axes.
FTXUI_EXPORT(DOM) Element flex(Element);
/// Allow the element to expand on both axes.
FTXUI_EXPORT(DOM) Element flex_grow(Element);
/// Allow the element to shrink on both axes.
FTXUI_EXPORT(DOM) Element flex_shrink(Element);
/// Set custom grow and shrink flex factors on both axes.
FTXUI_EXPORT(DOM) Decorator flex_factor(int grow, int shrink);
/// Set a custom grow flex factor on both axes.
FTXUI_EXPORT(DOM) Decorator flex_grow_factor(int grow);
/// Set a custom shrink flex factor on both axes.
FTXUI_EXPORT(DOM) Decorator flex_shrink_factor(int shrink);

/// Allow the element to expand or shrink on the X axis.
FTXUI_EXPORT(DOM) Element xflex(Element);
/// Allow the element to expand on the X axis.
FTXUI_EXPORT(DOM) Element xflex_grow(Element);
/// Allow the element to shrink on the X axis.
FTXUI_EXPORT(DOM) Element xflex_shrink(Element);
/// Set custom grow and shrink flex factors on the X axis.
FTXUI_EXPORT(DOM) Decorator xflex_factor(int grow, int shrink);
/// Set a custom grow flex factor on the X axis.
FTXUI_EXPORT(DOM) Decorator xflex_grow_factor(int grow);
/// Set a custom shrink flex factor on the X axis.
FTXUI_EXPORT(DOM) Decorator xflex_shrink_factor(int shrink);

/// Allow the element to expand or shrink on the Y axis.
FTXUI_EXPORT(DOM) Element yflex(Element);
/// Allow the element to expand on the Y axis.
FTXUI_EXPORT(DOM) Element yflex_grow(Element);
/// Allow the element to shrink on the Y axis.
FTXUI_EXPORT(DOM) Element yflex_shrink(Element);
/// Set custom grow and shrink flex factors on the Y axis.
FTXUI_EXPORT(DOM) Decorator yflex_factor(int grow, int shrink);
/// Set a custom grow flex factor on the Y axis.
FTXUI_EXPORT(DOM) Decorator yflex_grow_factor(int grow);
/// Set a custom shrink flex factor on the Y axis.
FTXUI_EXPORT(DOM) Decorator yflex_shrink_factor(int shrink);

/// Reset flex attributes so the element uses its minimum size.
FTXUI_EXPORT(DOM) Element notflex(Element);
/// Blank expandable element that fills remaining space.
FTXUI_EXPORT(DOM) Element filler();

// --- Size override ---

enum WidthOrHeight : uint8_t { WIDTH, HEIGHT };
enum Constraint : uint8_t { LESS_THAN, EQUAL, GREATER_THAN };
/// Override an element's width or height constraint.
FTXUI_EXPORT(DOM) Decorator size(WidthOrHeight, Constraint, int value);

// --- Frame ---
// A frame is a scrollable area. The internal area is potentially larger than
// the external one. The internal area is scrolled in order to make visible the
// focused element.

/// Make an element scrollable on both axes around the focused child.
FTXUI_EXPORT(DOM) Element frame(Element);
/// Make an element scrollable horizontally around the focused child.
FTXUI_EXPORT(DOM) Element xframe(Element);
/// Make an element scrollable vertically around the focused child.
FTXUI_EXPORT(DOM) Element yframe(Element);
/// Mark an element as focused so enclosing frames scroll to keep it visible.
FTXUI_EXPORT(DOM) Element focus(Element);
/// Deprecated alias for focus().
FTXUI_EXPORT(DOM) Element select(Element e);

// --- Cursor ---
// Similar to `focus`, but also change the terminal cursor shape.

/// Focus the element and display a steady block cursor.
FTXUI_EXPORT(DOM) Element focusCursorBlock(Element);
/// Focus the element and display a blinking block cursor.
FTXUI_EXPORT(DOM) Element focusCursorBlockBlinking(Element);
/// Focus the element and display a steady bar cursor.
FTXUI_EXPORT(DOM) Element focusCursorBar(Element);
/// Focus the element and display a blinking bar cursor.
FTXUI_EXPORT(DOM) Element focusCursorBarBlinking(Element);
/// Focus the element and display a steady underline cursor.
FTXUI_EXPORT(DOM) Element focusCursorUnderline(Element);
/// Focus the element and display a blinking underline cursor.
FTXUI_EXPORT(DOM) Element focusCursorUnderlineBlinking(Element);

// --- Misc ---

/// Display a vertical scroll indicator alongside a yframe/frame.
FTXUI_EXPORT(DOM) Element vscroll_indicator(Element);
/// Display a horizontal scroll indicator below an xframe/frame.
FTXUI_EXPORT(DOM) Element hscroll_indicator(Element);
/// Record the element's rendered bounding box into `box`.
FTXUI_EXPORT(DOM) Decorator reflect(Box& box);
/// Clear cells underneath the element before rendering (useful with dbox).
FTXUI_EXPORT(DOM) Element clear_under(Element element);

// --- Util ---

/// Center an element horizontally.
FTXUI_EXPORT(DOM) Element hcenter(Element);
/// Center an element vertically.
FTXUI_EXPORT(DOM) Element vcenter(Element);
/// Center an element both horizontally and vertically.
FTXUI_EXPORT(DOM) Element center(Element);
/// Align an element to the right.
FTXUI_EXPORT(DOM) Element align_right(Element);
/// No-op decorator returning the element unchanged.
FTXUI_EXPORT(DOM) Element nothing(Element element);

namespace Dimension {
/// Compute dimensions fitting the given element.
FTXUI_EXPORT(DOM) Dimensions Fit(Element&, bool extend_beyond_screen = false);
}  // namespace Dimension

}  // namespace ftxui

// Make container able to take any number of children as input.
#include "ftxui/dom/take_any_args.hpp"

// Include old definitions using wstring.
#include "ftxui/dom/deprecated.hpp"
#endif  // FTXUI_DOM_ELEMENTS_HPP
