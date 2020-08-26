#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

#include <functional>
#include <memory>

#include "ftxui/dom/node.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/screen/screen.hpp"

namespace ftxui {
class Node;
using Element = std::shared_ptr<Node>;
using Elements = std::vector<Element>;
using Decorator = std::function<Element(Element)>;
using GraphFunction = std::function<std::vector<int>(int, int)>;

// Pipe elements into decorator togethers.
// For instance the next lines are equivalents:
// -> text("ftxui") | bold | underlined
// -> underlined(bold(text(L"FTXUI")))
Element operator|(Element, Decorator);
Elements operator|(Elements, Decorator);
Decorator operator|(Decorator, Decorator);

// --- Widget ---
Element text(std::wstring text);
Element vtext(std::wstring text);
Element separator(void);
Element separator(Pixel);
Element gauge(float ratio);
Element border(Element);
Decorator borderWith(Pixel);
Element window(Element title, Element content);
Element spinner(int charset_index, size_t image_index);
Elements paragraph(std::wstring text);  // Use inside hflow(). Split by space.
Element graph(GraphFunction);

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

// --- Layout is
// Horizontal, Vertical or stacked set of elements.
Element hbox(Elements);
Element vbox(Elements);
Element dbox(Elements);
Element hflow(Elements);

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

// --- Frame ---
// A frame is a scrollable area. The internal area is potentially larger than
// the external one. The internal area is scrolled in order to make visible the
// focused element.
Element frame(Element);
Element xframe(Element);
Element yframe(Element);
Element focus(Element);
Element select(Element);

// --- Util --------------------------------------------------------------------
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);
Element align_right(Element);
Element nothing(Element element);

// Before drawing the |element| clear the pixel below. This is useful in
// combinaison with dbox.
Element clear_under(Element element);

// Make container able to take any number of children as input.
#include "take_any_args.hpp"
TAKE_ANY_ARGS(vbox)
TAKE_ANY_ARGS(hbox)
TAKE_ANY_ARGS(dbox)
TAKE_ANY_ARGS(hflow)

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_ELEMENTS_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
