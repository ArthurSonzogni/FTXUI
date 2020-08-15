#include <type_traits>

template <class T>
void Merge(Elements&, T) {}

template <>
inline void Merge(Elements& container, Element element) {
  container.push_back(std::move(element));
}

template <>
inline void Merge(Elements& container, Elements elements) {
  for (auto& element : elements)
    container.push_back(std::move(element));
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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
