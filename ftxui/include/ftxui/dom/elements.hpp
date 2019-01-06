#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

#include <functional>

#include "ftxui/dom/node.hpp"
#include "ftxui/screen/color.hpp"

namespace ftxui::dom {

using Element = std::unique_ptr<Node>;
using Decorator = std::function<Element(Element)>;
using Child = std::unique_ptr<Node>;
using Children = std::vector<Child>;
using Color = ftxui::screen::Color;

// --- Layout ----
Element vbox(Children);
Element hbox(Children);
Element dbox(Children);
Element filler();
Element flex(Element);

// --- Widget --
Element text(std::wstring text);
Element separator();
Element gauge(float ratio);
Element frame(Element);
Element window(Child title, Child content);

// -- Decorator ---
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element blink(Element);
Element color(Color, Element);
Element bgcolor(Color, Element);
Decorator color(Color);
Decorator bgcolor(Color);

// --- Util ---
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);

// --- Util ---
Element nothing(Element element);

// Pipe elements into decorator togethers.
// Examples: text("ftxui") | bold | underlined;
Element operator|(Element, Decorator);
Decorator operator|(Decorator, Decorator);

// Make container able to take any number of children as input.
#include "take_any_args.hpp"
TAKE_ANY_ARGS(vbox)
TAKE_ANY_ARGS(hbox)
TAKE_ANY_ARGS(dbox)

};  // namespace ftxui::dom

#endif /* end of include guard: FTXUI_DOM_ELEMENTS_HPP */
