#ifndef FTXUI_CORE_DOM_ELEMENTS_HPP
#define FTXUI_CORE_DOM_ELEMENTS_HPP

#include "ftxui/core/dom/node.hpp"
#include <initializer_list>

namespace ftxui {
namespace dom {

using Child = std::unique_ptr<Node>;
using Children = std::vector<std::unique_ptr<Node>>;

std::unique_ptr<Node> vbox(Children);
std::unique_ptr<Node> hbox(Children);
std::unique_ptr<Node> text(std::wstring text);
std::unique_ptr<Node> flex();

template <class... Args>
std::vector<Child> unpack(Args... args) {
  std::vector<Child> vec;
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
