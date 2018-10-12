#ifndef FTXUI_DOM_ELEMENTS_HPP
#define FTXUI_DOM_ELEMENTS_HPP

#include "ftxui/color.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {
namespace dom {

using Element = std::unique_ptr<Node>;
using Child = std::unique_ptr<Node>;
using Children = std::vector<Child>;

// --- Layout ----
Element vbox(Children);
Element hbox(Children);
Element flex();

// --- Widget --
Element text(std::wstring text);
Element separator();
Element gauge(float ratio);
Element frame(Child);
Element frame(Child title, Child content);

// -- Decorator (Style) ---
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element color(Color, Element);
Element bgcolor(Color, Element);

// --- Decorator ---
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);
Element flex(Element);

template <class... Args>
Children unpack(Args... args) {
  Children vec;
  (vec.push_back(std::forward<Args>(args)), ...);
  return vec;
}

template <class... Args>
Element vbox(Args... children) {
  return vbox(unpack(std::forward<Args>(children)...));
}

template <class... Args>
Element hbox(Args... children) {
  return hbox(unpack(std::forward<Args>(children)...));
}

};  // namespace dom
};  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_ELEMENTS_HPP */
