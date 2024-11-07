// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>   // for min
#include <functional>  // for function
#include <memory>      // for __shared_ptr_access, make_unique
#include <utility>     // for move

#include "ftxui/dom/elements.hpp"  // for Element, Decorator, Elements, operator|, Fit, emptyElement, nothing, operator|=
#include "ftxui/dom/node.hpp"      // for Node, Node::Status
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
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
  return compose(std::move(a),  //
                 std::move(b));
}

/// @brief From a set of element, apply a decorator to every elements.
/// @return the set of decorated element.
/// @ingroup dom
Elements operator|(Elements elements, Decorator decorator) {  // NOLINT
  Elements output;
  output.reserve(elements.size());
  for (auto& it : elements) {
    output.push_back(std::move(it) | decorator);
  }
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
Element operator|(Element element, Decorator decorator) {  // NOLINT
  return decorator(std::move(element));
}

/// @brief Apply a decorator to an element.
/// @return the decorated element.
/// @ingroup dom
///
/// ### Example
///
/// Both of these are equivalent:
/// ```cpp
/// auto element = text("Hello");
/// element |= bold;
/// ```
Element& operator|=(Element& e, Decorator d) {
  e = e | std::move(d);
  return e;
}

/// The minimal dimension that will fit the given element.
/// @see Fixed
/// @see Full
Dimensions Dimension::Fit(Element& e, bool extend_beyond_screen) {
  const Dimensions fullsize = Dimension::Full();
  Box box;
  box.x_min = 0;
  box.y_min = 0;
  box.x_max = fullsize.dimx;
  box.y_max = fullsize.dimy;

  Node::Status status;
  e->Check(&status);
  const int max_iteration = 20;
  while (status.need_iteration && status.iteration < max_iteration) {
    e->ComputeRequirement();

    // Don't give the element more space than it needs:
    box.x_max = std::min(box.x_max, e->requirement().min_x);
    box.y_max = e->requirement().min_y;
    if (!extend_beyond_screen) {
      box.y_max = std::min(box.y_max, fullsize.dimy);
    }

    e->SetBox(box);
    status.need_iteration = false;
    status.iteration++;
    e->Check(&status);

    if (!status.need_iteration) {
      break;
    }
    // Increase the size of the box until it fits...
    box.x_max = std::min(e->requirement().min_x, fullsize.dimx);
    box.y_max = e->requirement().min_y;

    // ... but don't go beyond the screen size:
    if (!extend_beyond_screen) {
      box.y_max = std::min(box.y_max, fullsize.dimy);
    }
  }

  return {
      box.x_max,
      box.y_max,
  };
}

/// An element of size 0x0 drawing nothing.
/// @ingroup dom
Element emptyElement() {
  class Impl : public Node {
    void ComputeRequirement() override {
      requirement_.min_x = 0;
      requirement_.min_y = 0;
    }
  };
  return std::make_unique<Impl>();
}

}  // namespace ftxui
