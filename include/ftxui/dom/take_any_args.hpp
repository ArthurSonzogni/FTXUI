// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_TAKE_ANY_ARGS_HPP
#define FTXUI_DOM_TAKE_ANY_ARGS_HPP

// IWYU pragma: private, include "ftxui/dom/elements.hpp"
#include <concepts>
#include <ftxui/dom/node.hpp>
#include <queue>
#include <ranges>
#include <stack>

namespace ftxui {

template <class T>
void Merge(Elements& /*container*/, T /*element*/) {}

template <>
inline void Merge(Elements& container, Element element) {
  container.push_back(std::move(element));
}

template <class Container>
concept ElementRange =
    std::ranges::range<Container> &&
    std::same_as<std::ranges::range_value_t<Container>, Element>;

template <ElementRange T>
inline void Merge(Elements& container, T elements) {
  for (auto& element : elements) {
    container.push_back(std::move(element));
  }
}

template <>
inline void Merge(Elements& container, std::stack<Element> elements) {
  while (!elements.empty()) {
    container.push_back(std::move(elements.top()));
    elements.pop();
  }
}

template <>
inline void Merge(Elements& container, std::queue<Element> elements) {
  while (!elements.empty()) {
    container.push_back(std::move(elements.back()));
    elements.pop();
  }
}

// Turn a set of arguments into a vector.
template <class... Args>
Elements unpack(Args... args) {
  std::vector<Element> vec;
  (Merge(vec, std::move(args)), ...);
  return vec;
}

// Make |container| able to take any number of argments.
#define TAKE_ANY_ARGS(container)                               \
  template <class... Args>                                     \
  Element container(Args... children) {                        \
    return container(unpack(std::forward<Args>(children)...)); \
  }

TAKE_ANY_ARGS(vbox)
TAKE_ANY_ARGS(hbox)
TAKE_ANY_ARGS(dbox)
TAKE_ANY_ARGS(hflow)
}  // namespace ftxui

#endif  // FTXUI_DOM_TAKE_ANY_ARGS_HPP
