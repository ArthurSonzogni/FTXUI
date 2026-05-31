// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

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
enum BorderStyle {
  LIGHT,
  DASHED,
  HEAVY,
  DOUBLE,
  ROUNDED,
  EMPTY,
};

// Pipe elements into decorator together.
// For instance the next lines are equivalents:
// -> text("ftxui") | bold | underlined
// -> underlined(bold(text("FTXUI")))
FTXUI_EXPORT(DOM) Element operator|(Element, Decorator);
FTXUI_EXPORT(DOM) Element& operator|=(Element&, Decorator);
FTXUI_EXPORT(DOM) Elements operator|(Elements, Decorator);
FTXUI_EXPORT(DOM) Decorator operator|(Decorator, Decorator);

// --- Widget ---
FTXUI_EXPORT(DOM) Element text(std::string_view text);
FTXUI_EXPORT(DOM) Element vtext(std::string_view text);
FTXUI_EXPORT(DOM) Element separator();
FTXUI_EXPORT(DOM) Element separatorLight();
FTXUI_EXPORT(DOM) Element separatorDashed();
FTXUI_EXPORT(DOM) Element separatorHeavy();
FTXUI_EXPORT(DOM) Element separatorDouble();
FTXUI_EXPORT(DOM) Element separatorEmpty();
FTXUI_EXPORT(DOM) Element separatorStyled(BorderStyle);
FTXUI_EXPORT(DOM) Element separator(Cell);
FTXUI_EXPORT(DOM) Element separatorCharacter(std::string_view);
FTXUI_EXPORT(DOM)
Element separatorHSelector(float left,
                           float right,
                           Color unselected_color,
                           Color selected_color);
FTXUI_EXPORT(DOM)
Element separatorVSelector(float up,
                           float down,
                           Color unselected_color,
                           Color selected_color);
FTXUI_EXPORT(DOM) Element gauge(float progress);
FTXUI_EXPORT(DOM) Element gaugeLeft(float progress);
FTXUI_EXPORT(DOM) Element gaugeRight(float progress);
FTXUI_EXPORT(DOM) Element gaugeUp(float progress);
FTXUI_EXPORT(DOM) Element gaugeDown(float progress);
FTXUI_EXPORT(DOM) Element gaugeDirection(float progress, Direction direction);
FTXUI_EXPORT(DOM) Element border(Element);
FTXUI_EXPORT(DOM) Element borderLight(Element);
FTXUI_EXPORT(DOM) Element borderDashed(Element);
FTXUI_EXPORT(DOM) Element borderHeavy(Element);
FTXUI_EXPORT(DOM) Element borderDouble(Element);
FTXUI_EXPORT(DOM) Element borderRounded(Element);
FTXUI_EXPORT(DOM) Element borderEmpty(Element);
FTXUI_EXPORT(DOM) Decorator borderStyled(BorderStyle);
FTXUI_EXPORT(DOM) Decorator borderStyled(BorderStyle, Color);
FTXUI_EXPORT(DOM) Decorator borderStyled(Color);
FTXUI_EXPORT(DOM) Decorator borderWith(const Cell&);
FTXUI_EXPORT(DOM)
Element window(Element title, Element content, BorderStyle border = ROUNDED);
FTXUI_EXPORT(DOM) Element spinner(int charset_index, size_t image_index);
FTXUI_EXPORT(DOM) Element paragraph(std::string_view text);
FTXUI_EXPORT(DOM) Element paragraphAlignLeft(std::string_view text);
FTXUI_EXPORT(DOM) Element paragraphAlignRight(std::string_view text);
FTXUI_EXPORT(DOM) Element paragraphAlignCenter(std::string_view text);
FTXUI_EXPORT(DOM) Element paragraphAlignJustify(std::string_view text);
FTXUI_EXPORT(DOM) Element graph(GraphFunction);
FTXUI_EXPORT(DOM) Element emptyElement();
FTXUI_EXPORT(DOM) Element canvas(ConstRef<Canvas>);
FTXUI_EXPORT(DOM)
Element canvas(int width, int height, std::function<void(Canvas&)>);
FTXUI_EXPORT(DOM) Element canvas(std::function<void(Canvas&)>);

// -- Decorator ---
FTXUI_EXPORT(DOM) Element bold(Element);
FTXUI_EXPORT(DOM) Element dim(Element);
FTXUI_EXPORT(DOM) Element italic(Element);
FTXUI_EXPORT(DOM) Element inverted(Element);
FTXUI_EXPORT(DOM) Element underlined(Element);
FTXUI_EXPORT(DOM) Element underlinedDouble(Element);
FTXUI_EXPORT(DOM) Element blink(Element);
FTXUI_EXPORT(DOM) Element strikethrough(Element);
FTXUI_EXPORT(DOM) Decorator color(Color);
FTXUI_EXPORT(DOM) Decorator bgcolor(Color);
FTXUI_EXPORT(DOM) Decorator color(const LinearGradient&);
FTXUI_EXPORT(DOM) Decorator bgcolor(const LinearGradient&);
FTXUI_EXPORT(DOM) Element color(Color, Element);
FTXUI_EXPORT(DOM) Element bgcolor(Color, Element);
FTXUI_EXPORT(DOM) Element color(const LinearGradient&, Element);
FTXUI_EXPORT(DOM) Element bgcolor(const LinearGradient&, Element);
FTXUI_EXPORT(DOM) Decorator focusPosition(int x, int y);
FTXUI_EXPORT(DOM) Decorator focusPositionRelative(float x, float y);
FTXUI_EXPORT(DOM) Element automerge(Element child);
FTXUI_EXPORT(DOM) Decorator hyperlink(std::string_view link);
FTXUI_EXPORT(DOM) Element hyperlink(std::string_view link, Element child);
FTXUI_EXPORT(DOM) Element selectionStyleReset(Element);
FTXUI_EXPORT(DOM) Decorator selectionColor(Color foreground);
FTXUI_EXPORT(DOM) Decorator selectionBackgroundColor(Color foreground);
FTXUI_EXPORT(DOM) Decorator selectionForegroundColor(Color foreground);
FTXUI_EXPORT(DOM) Decorator selectionStyle(std::function<void(Cell&)> style);

// --- Layout is
// Horizontal, Vertical or stacked set of elements.
FTXUI_EXPORT(DOM) Element hbox(Elements);
FTXUI_EXPORT(DOM) Element vbox(Elements);
FTXUI_EXPORT(DOM) Element dbox(Elements);
FTXUI_EXPORT(DOM)
Element flexbox(Elements, FlexboxConfig config = FlexboxConfig());
FTXUI_EXPORT(DOM) Element gridbox(std::vector<Elements> lines);

FTXUI_EXPORT(DOM)
Element hflow(Elements);  // Helper: default flexbox with row direction.
FTXUI_EXPORT(DOM)
Element vflow(Elements);  // Helper: default flexbox with column direction.

// -- Flexibility ---
// Define how to share the remaining space when not all of it is used inside a
// container.
FTXUI_EXPORT(DOM) Element flex(Element);  // Expand/Minimize if possible/needed.
FTXUI_EXPORT(DOM) Element flex_grow(Element);    // Expand element if possible.
FTXUI_EXPORT(DOM) Element flex_shrink(Element);  // Minimize element if needed.

FTXUI_EXPORT(DOM)
Element xflex(Element);  // Expand/Minimize if possible/needed on X axis.
FTXUI_EXPORT(DOM)
Element xflex_grow(Element);  // Expand element if possible on X axis.
FTXUI_EXPORT(DOM)
Element xflex_shrink(Element);  // Minimize element if needed on X axis.

FTXUI_EXPORT(DOM)
Element yflex(Element);  // Expand/Minimize if possible/needed on Y axis.
FTXUI_EXPORT(DOM)
Element yflex_grow(Element);  // Expand element if possible on Y axis.
FTXUI_EXPORT(DOM)
Element yflex_shrink(Element);  // Minimize element if needed on Y axis.

FTXUI_EXPORT(DOM) Element notflex(Element);  // Reset the flex attribute.
FTXUI_EXPORT(DOM) Element filler();          // A blank expandable element.

// -- Size override;
enum WidthOrHeight { WIDTH, HEIGHT };
enum Constraint { LESS_THAN, EQUAL, GREATER_THAN };
FTXUI_EXPORT(DOM) Decorator size(WidthOrHeight, Constraint, int value);

// --- Frame ---
// A frame is a scrollable area. The internal area is potentially larger than
// the external one. The internal area is scrolled in order to make visible the
// focused element.
FTXUI_EXPORT(DOM) Element frame(Element);
FTXUI_EXPORT(DOM) Element xframe(Element);
FTXUI_EXPORT(DOM) Element yframe(Element);
FTXUI_EXPORT(DOM) Element focus(Element);
FTXUI_EXPORT(DOM) Element select(Element e);  // Deprecated - Alias for focus.

// --- Cursor ---
// Those are similar to `focus`, but also change the shape of the cursor.
FTXUI_EXPORT(DOM) Element focusCursorBlock(Element);
FTXUI_EXPORT(DOM) Element focusCursorBlockBlinking(Element);
FTXUI_EXPORT(DOM) Element focusCursorBar(Element);
FTXUI_EXPORT(DOM) Element focusCursorBarBlinking(Element);
FTXUI_EXPORT(DOM) Element focusCursorUnderline(Element);
FTXUI_EXPORT(DOM) Element focusCursorUnderlineBlinking(Element);

// --- Misc ---
FTXUI_EXPORT(DOM) Element vscroll_indicator(Element);
FTXUI_EXPORT(DOM) Element hscroll_indicator(Element);
FTXUI_EXPORT(DOM) Decorator reflect(Box& box);
FTXUI_EXPORT(DOM) Decorator reflect(std::function<void(Box)> callback);
// Before drawing the |element| clear the pixel below. This is useful in
// combination with dbox.
FTXUI_EXPORT(DOM) Element clear_under(Element element);

// --- Util --------------------------------------------------------------------
FTXUI_EXPORT(DOM) Element hcenter(Element);
FTXUI_EXPORT(DOM) Element vcenter(Element);
FTXUI_EXPORT(DOM) Element center(Element);
FTXUI_EXPORT(DOM) Element align_right(Element);
FTXUI_EXPORT(DOM) Element nothing(Element element);

namespace Dimension {
FTXUI_EXPORT(DOM) Dimensions Fit(Element&, bool extend_beyond_screen = false);
}  // namespace Dimension

}  // namespace ftxui

// Make container able to take any number of children as input.
#include "ftxui/dom/take_any_args.hpp"

// Include old definitions using wstring.
#include "ftxui/dom/deprecated.hpp"
#endif  // FTXUI_DOM_ELEMENTS_HPP
