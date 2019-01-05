#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

#include <functional>

#include "ftxui/color.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {
namespace dom {

using Element = std::unique_ptr<Node>;
using Decorator = std::function<Element(Element)>;
using Child = std::unique_ptr<Node>;
using Children = std::vector<Child>;

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

// Make container able to take several children.
template <class... Args>
Children unpack(Args... args) {
  Children vec;
  (vec.push_back(std::forward<Args>(args)), ...);
  return vec;
}

#define TAKE_ANY_ARGS(container) \
  template <class... Args> \
  Element container(Args... children) { \
    return container(unpack(std::forward<Args>(children)...)); \
  } \

TAKE_ANY_ARGS(vbox)
TAKE_ANY_ARGS(hbox)
TAKE_ANY_ARGS(dbox)

};  // namespace dom
};  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_ELEMENTS_HPP */
