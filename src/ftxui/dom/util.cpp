#include <algorithm>   // for min
#include <functional>  // for function
#include <memory>      // for __shared_ptr_access
#include <utility>     // for move
#include <vector>      // for vector

#include "ftxui/dom/elements.hpp"  // for Element, Decorator, Elements, operator|, Fit, nothing
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/screen.hpp"    // for Full
#include "ftxui/screen/terminal.hpp"  // for Dimensions

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
/// bold(text("Hello"));
/// ```
/// ```cpp
/// text("Hello") | bold;
/// ```
Element operator|(Element element, Decorator decorator) {
  return decorator(std::move(element));
}

/// The minimal dimension that will fit the given element.
/// @see Fixed
/// @see Full
Dimensions Dimension::Fit(Element& e) {
  e->ComputeRequirement();
  Dimensions size = Dimension::Full();
  return {std::min(e->requirement().min_x, size.dimx),
          std::min(e->requirement().min_y, size.dimy)};
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
