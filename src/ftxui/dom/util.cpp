#include "ftxui/dom/elements.hpp"

namespace ftxui {

namespace {
Decorator compose(Decorator a, Decorator b) {
  return [a = std::move(a), b = std::move(b)](Element element) {
    return b(a(std::move(element)));
  };
}
}  // namespace

/// @brief A decoration doing absolutely nothing.
/// @ingroup dom
Element nothing(Element element) {
  return element;
}

/// @brief Compose two decorator into one.
/// @ingroup dom
///
/// ### Example
///
/// ```cpp
/// auto decorator = bold | blink;
/// ```
Decorator operator|(Decorator a, Decorator b) {
  return compose(a, b);
}

/// @brief From a set of element, apply a decorator to every elements.
/// @return the set of decorated element.
/// @ingroup dom
Elements operator|(Elements elements, Decorator decorator) {
  Elements output;
  for (auto& it : elements)
    output.push_back(std::move(it) | decorator);
  return output;
}

/// @brief From an element, apply a decorator.
/// @return the decorated element.
/// @ingroup dom
///
/// ### Example
///
/// Both of these are equivalent:
/// ```cpp
/// bold(text(L"Hello"));
/// ```
/// ```cpp
/// text(L"Hello") | bold;
/// ```
Element operator|(Element element, Decorator decorator) {
  return decorator(std::move(element));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
