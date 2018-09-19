#ifndef FTXUI_CORE_DOM_ELEMENTS_HPP
#define FTXUI_CORE_DOM_ELEMENTS_HPP

#include "ftxui/core/dom/node.hpp"
#include <initializer_list>

namespace ftxui {
namespace dom {

using Element = std::unique_ptr<Node>;
using Children = std::vector<std::unique_ptr<Node>>;


// --- Layout ----
std::unique_ptr<Node> vbox(Children);
std::unique_ptr<Node> hbox(Children);
std::unique_ptr<Node> flex();
std::unique_ptr<Node> flex(Element);

// --- Widget --
std::unique_ptr<Node> text(std::wstring text);
std::unique_ptr<Node> separator();
std::unique_ptr<Node> gauge(float ratio);

// --- Decorator ---
std::unique_ptr<Node> hcenter(Element);
std::unique_ptr<Node> vcenter(Element);
std::unique_ptr<Node> center(Element);

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
