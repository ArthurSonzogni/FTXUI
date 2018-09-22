#ifndef FTXUI_CORE_DOM_ELEMENTS_HPP
#define FTXUI_CORE_DOM_ELEMENTS_HPP

#include "ftxui/core/dom/node.hpp"
#include <initializer_list>

namespace ftxui {
namespace dom {

using Element = std::unique_ptr<Node>;
using Child = std::unique_ptr<Node>;
using Children = std::vector<std::unique_ptr<Node>>;


// --- Layout ----
Element vbox(Children);
Element hbox(Children);
Element flex();

// --- Widget --
Element text(std::wstring text);
Element separator();
Element gauge(float ratio);
Element frame(Child);
Element frame(std::wstring title, Child);

// --- Decorator ---
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);
Element flex(Element);

template <class... Args>
std::vector<Element> unpack(Args... args) {
  std::vector<Element> vec;
  (vec.push_back(std::forward<Args>(args)), ...);
  return vec;
}

template <class... Args>
std::unique_ptr<Node> vbox(Args... children) {
  return vbox(unpack(std::forward<Args>(children)...));
}

template <class... Args>
std::unique_ptr<Node> hbox(Args... children) {
  return hbox(unpack(std::forward<Args>(children)...));
}

};  // namespace dom
};  // namespace ftxui

#endif /* end of include guard: FTXUI_CORE_DOM_ELEMENTS_HPP */
