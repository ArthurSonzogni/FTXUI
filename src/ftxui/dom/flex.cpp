// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>   // for make_shared, __shared_ptr_access
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"  // for Element, unpack, filler, flex, flex_grow, flex_shrink, notflex, xflex, xflex_grow, xflex_shrink, yflex, yflex_grow, yflex_shrink
#include "ftxui/dom/node.hpp"      // for Elements, Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box

namespace ftxui {

namespace {

class Flex : public Node {
 public:
  Flex(int grow_x, int grow_y, int shrink_x, int shrink_y)
      : grow_x_(grow_x),
        grow_y_(grow_y),
        shrink_x_(shrink_x),
        shrink_y_(shrink_y) {}
  Flex(Element child, int grow_x, int grow_y, int shrink_x, int shrink_y)
      : Node(unpack(std::move(child))),
        grow_x_(grow_x),
        grow_y_(grow_y),
        shrink_x_(shrink_x),
        shrink_y_(shrink_y) {}

  void ComputeRequirement() override {
    requirement_.min_x = 0;
    requirement_.min_y = 0;
    if (!children_.empty()) {
      children_[0]->ComputeRequirement();
      requirement_ = children_[0]->requirement();
    }
    if (grow_x_ != -1) {
      requirement_.flex_grow_x = grow_x_;
    }
    if (grow_y_ != -1) {
      requirement_.flex_grow_y = grow_y_;
    }
    if (shrink_x_ != -1) {
      requirement_.flex_shrink_x = shrink_x_;
    }
    if (shrink_y_ != -1) {
      requirement_.flex_shrink_y = shrink_y_;
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    if (children_.empty()) {
      return;
    }
    children_[0]->SetBox(box);
  }

  int grow_x_;
  int grow_y_;
  int shrink_x_;
  int shrink_y_;
};

}  // namespace

/// @brief An element that will expand proportionally to the space left in
/// a container.
/// @ingroup dom
Element filler() {
  return std::make_shared<Flex>(1, 1, 1, 1);
}

/// @brief Make a child element to expand proportionally to the space left in a
/// container.
/// @ingroup dom
///
/// #### Examples:
///
/// ~~~cpp
///   hbox({
///     text("left") | border ,
///     text("middle") | border | flex,
///     text("right") | border,
///   });
/// ~~~
///
/// #### Output:
///
/// ~~~bash
/// ┌────┐┌─────────────────────────────────────────────────────────┐┌─────┐
/// │left││middle                                                   ││right│
/// └────┘└─────────────────────────────────────────────────────────┘└─────┘
/// ~~~
Element flex(Element child) {
  return std::make_shared<Flex>(std::move(child), 1, 1, 1, 1);
}

/// @brief Expand/Minimize if possible/needed on the X axis.
/// @ingroup dom
Element xflex(Element child) {
  return std::make_shared<Flex>(std::move(child), 1, -1, 1, -1);
}

/// @brief Expand/Minimize if possible/needed on the Y axis.
/// @ingroup dom
Element yflex(Element child) {
  return std::make_shared<Flex>(std::move(child), -1, 1, -1, 1);
}

/// @brief Expand if possible.
/// @ingroup dom
Element flex_grow(Element child) {
  return std::make_shared<Flex>(std::move(child), 1, 1, -1, -1);
}

/// @brief Expand if possible on the X axis.
/// @ingroup dom
Element xflex_grow(Element child) {
  return std::make_shared<Flex>(std::move(child), 1, -1, -1, -1);
}

/// @brief Expand if possible on the Y axis.
/// @ingroup dom
Element yflex_grow(Element child) {
  return std::make_shared<Flex>(std::move(child), -1, 1, -1, -1);
}

/// @brief Minimize if needed.
/// @ingroup dom
Element flex_shrink(Element child) {
  return std::make_shared<Flex>(std::move(child), -1, -1, 1, 1);
}

/// @brief Minimize if needed on the X axis.
/// @ingroup dom
Element xflex_shrink(Element child) {
  return std::make_shared<Flex>(std::move(child), -1, -1, 1, -1);
}

/// @brief Minimize if needed on the Y axis.
/// @ingroup dom
Element yflex_shrink(Element child) {
  return std::make_shared<Flex>(std::move(child), -1, -1, -1, 1);
}

/// @brief Make the element not flexible.
/// @ingroup dom
Element notflex(Element child) {
  return std::make_shared<Flex>(std::move(child), 0, 0, 0, 0);
}

Decorator flex_factor(int grow, int shrink) {
  return [grow, shrink](Element child) {
    return std::make_shared<Flex>(std::move(child), grow, grow, shrink, shrink);
  };
}

Decorator flex_grow_factor(int grow) {
  return [grow](Element child) {
    return std::make_shared<Flex>(std::move(child), grow, grow, -1, -1);
  };
}

Decorator flex_shrink_factor(int shrink) {
  return [shrink](Element child) {
    return std::make_shared<Flex>(std::move(child), -1, -1, shrink, shrink);
  };
}

Decorator xflex_factor(int grow, int shrink) {
  return [grow, shrink](Element child) {
    return std::make_shared<Flex>(std::move(child), grow, -1, shrink, -1);
  };
}

Decorator xflex_grow_factor(int grow) {
  return [grow](Element child) {
    return std::make_shared<Flex>(std::move(child), grow, -1, -1, -1);
  };
}

Decorator xflex_shrink_factor(int shrink) {
  return [shrink](Element child) {
    return std::make_shared<Flex>(std::move(child), -1, -1, shrink, -1);
  };
}

Decorator yflex_factor(int grow, int shrink) {
  return [grow, shrink](Element child) {
    return std::make_shared<Flex>(std::move(child), -1, grow, -1, shrink);
  };
}

Decorator yflex_grow_factor(int grow) {
  return [grow](Element child) {
    return std::make_shared<Flex>(std::move(child), -1, grow, -1, -1);
  };
}

Decorator yflex_shrink_factor(int shrink) {
  return [shrink](Element child) {
    return std::make_shared<Flex>(std::move(child), -1, -1, -1, shrink);
  };
}

}  // namespace ftxui
