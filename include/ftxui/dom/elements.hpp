#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

#include <functional>
#include <memory>

#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/flexbox_config.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/box.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/terminal.hpp"
#include "ftxui/util/ref.hpp"

namespace ftxui {
class Node;
using Element = std::shared_ptr<Node>;
using Elements = std::vector<Element>;
using Decorator = std::function<Element(Element)>;
using GraphFunction = std::function<std::vector<int>(int, int)>;

enum BorderStyle { LIGHT, HEAVY, DOUBLE, ROUNDED, EMPTY };
enum class GaugeDirection { Left, Up, Right, Down };

// Pipe elements into decorator togethers.
// For instance the next lines are equivalents:
// -> text("ftxui") | bold | underlined
// -> underlined(bold(text("FTXUI")))
Element operator|(Element, Decorator);
Element& operator|=(Element&, Decorator);
Elements operator|(Elements, Decorator);
Decorator operator|(Decorator, Decorator);

// --- Widget ---
Element text(std::string text);
Element vtext(std::string text);
Element separator();
Element separatorLight();
Element separatorHeavy();
Element separatorDouble();
Element separatorEmpty();
Element separatorStyled(BorderStyle);
Element separator(Pixel);
Element separatorCharacter(std::string);
Element separatorHSelector(float left,
                           float right,
                           Color unselected_color,
                           Color selected_color);
Element separatorVSelector(float up,
                           float down,
                           Color unselected_color,
                           Color selected_color);
Element gauge(float progress);
Element gaugeLeft(float progress);
Element gaugeRight(float progress);
Element gaugeUp(float progress);
Element gaugeDown(float progress);
Element gaugeDirection(float progress, GaugeDirection);
Element border(Element);
Element borderLight(Element);
Element borderHeavy(Element);
Element borderDouble(Element);
Element borderRounded(Element);
Element borderEmpty(Element);
Decorator borderStyled(BorderStyle);
Decorator borderWith(const Pixel&);
Element window(Element title, Element content);
Element spinner(int charset_index, size_t image_index);
Element paragraph(const std::string& text);
Element paragraphAlignLeft(const std::string& text);
Element paragraphAlignRight(const std::string& text);
Element paragraphAlignCenter(const std::string& text);
Element paragraphAlignJustify(const std::string& text);
Element graph(GraphFunction);
Element emptyElement();
Element canvas(ConstRef<Canvas>);
Element canvas(int width, int height, std::function<void(Canvas&)>);
Element canvas(std::function<void(Canvas&)>);

// -- Decorator ---
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element blink(Element);
Decorator color(Color);
Decorator bgcolor(Color);
Element color(Color, Element);
Element bgcolor(Color, Element);
Decorator focusPosition(int x, int y);
Decorator focusPositionRelative(float x, float y);
Element automerge(Element child);

// --- Layout is
// Horizontal, Vertical or stacked set of elements.
Element hbox(Elements);
Element vbox(Elements);
Element dbox(Elements);
Element flexbox(Elements, FlexboxConfig config = FlexboxConfig());
Element gridbox(std::vector<Elements> lines);

Element hflow(Elements);  // Helper: default flexbox with row direction.
Element vflow(Elements);  // Helper: default flexbox with column direction.

// -- Flexibility ---
// Define how to share the remaining space when not all of it is used inside a
// container.
Element flex(Element);         // Expand/Minimize if possible/needed.
Element flex_grow(Element);    // Expand element if possible.
Element flex_shrink(Element);  // Minimize element if needed.

Element xflex(Element);         // Expand/Minimize if possible/needed on X axis.
Element xflex_grow(Element);    // Expand element if possible on X axis.
Element xflex_shrink(Element);  // Minimize element if needed on X axis.

Element yflex(Element);         // Expand/Minimize if possible/needed on Y axis.
Element yflex_grow(Element);    // Expand element if possible on Y axis.
Element yflex_shrink(Element);  // Minimize element if needed on Y axis.

Element notflex(Element);  // Reset the flex attribute.
Element filler();          // A blank expandable element.

// -- Size override;
enum Direction { WIDTH, HEIGHT };
enum Constraint { LESS_THAN, EQUAL, GREATER_THAN };
Decorator size(Direction, Constraint, int value);

// --
Decorator reflect(Box& box);

// --- Frame ---
// A frame is a scrollable area. The internal area is potentially larger than
// the external one. The internal area is scrolled in order to make visible the
// focused element.
Element frame(Element);
Element xframe(Element);
Element yframe(Element);
Element focus(Element);
Element select(Element);

Element vscroll_indicator(Element);

// --- Util --------------------------------------------------------------------
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);
Element align_right(Element);
Element nothing(Element element);

// Before drawing the |element| clear the pixel below. This is useful in
// combinaison with dbox.
Element clear_under(Element element);

namespace Dimension {
Dimensions Fit(Element&);
}  // namespace Dimension

}  // namespace ftxui

// Make container able to take any number of children as input.
#include "ftxui/dom/take_any_args.hpp"

// Include old definitions using wstring.
#include "ftxui/dom/deprecated.hpp"
#endif  // FTXUI_DOM_ELEMENTS_HPP

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
