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

using FlexFunction = void (*)(Requirement&);

void function_flex_grow(Requirement& r) {
  r.flex_grow_x = 1;
  r.flex_grow_y = 1;
}

void function_xflex_grow(Requirement& r) {
  r.flex_grow_x = 1;
}

void function_yflex_grow(Requirement& r) {
  r.flex_grow_y = 1;
}

void function_flex_shrink(Requirement& r) {
  r.flex_shrink_x = 1;
  r.flex_shrink_y = 1;
}

void function_xflex_shrink(Requirement& r) {
  r.flex_shrink_x = 1;
}

void function_yflex_shrink(Requirement& r) {
  r.flex_shrink_y = 1;
}

void function_flex(Requirement& r) {
  r.flex_grow_x = 1;
  r.flex_grow_y = 1;
  r.flex_shrink_x = 1;
  r.flex_shrink_y = 1;
}

void function_xflex(Requirement& r) {
  r.flex_grow_x = 1;
  r.flex_shrink_x = 1;
}

void function_yflex(Requirement& r) {
  r.flex_grow_y = 1;
  r.flex_shrink_y = 1;
}

void function_not_flex(Requirement& r) {
  r.flex_grow_x = 0;
  r.flex_grow_y = 0;
  r.flex_shrink_x = 0;
  r.flex_shrink_y = 0;
}

class Flex : public Node {
 public:
  explicit Flex(FlexFunction f) : f_(f) {}
  Flex(FlexFunction f, Element child) : Node(unpack(std::move(child))), f_(f) {}
  void ComputeRequirement() override {
    requirement_.min_x = 0;
    requirement_.min_y = 0;
    if (!children_.empty()) {
      children_[0]->ComputeRequirement();
      requirement_ = children_[0]->requirement();
    }
    f_(requirement_);
  }

  void SetBox(Box box) override {
    if (children_.empty()) {
      return;
    }
    children_[0]->SetBox(box);
  }

  FlexFunction f_;
};

}  // namespace

/// @brief An element that will take expand proportionally to the space left in
/// a container.
/// @ingroup dom
Element filler() {
  return std::make_shared<Flex>(function_flex);
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
  return std::make_shared<Flex>(function_flex, std::move(child));
}

/// @brief Expand/Minimize if possible/needed on the X axis.
/// @ingroup dom
Element xflex(Element child) {
  return std::make_shared<Flex>(function_xflex, std::move(child));
}

/// @brief Expand/Minimize if possible/needed on the Y axis.
/// @ingroup dom
Element yflex(Element child) {
  return std::make_shared<Flex>(function_yflex, std::move(child));
}

/// @brief Expand if possible.
/// @ingroup dom
Element flex_grow(Element child) {
  return std::make_shared<Flex>(function_flex_grow, std::move(child));
}

/// @brief Expand if possible on the X axis.
/// @ingroup dom
Element xflex_grow(Element child) {
  return std::make_shared<Flex>(function_xflex_grow, std::move(child));
}

/// @brief Expand if possible on the Y axis.
/// @ingroup dom
Element yflex_grow(Element child) {
  return std::make_shared<Flex>(function_yflex_grow, std::move(child));
}

/// @brief Minimize if needed.
/// @ingroup dom
Element flex_shrink(Element child) {
  return std::make_shared<Flex>(function_flex_shrink, std::move(child));
}

/// @brief Minimize if needed on the X axis.
/// @ingroup dom
Element xflex_shrink(Element child) {
  return std::make_shared<Flex>(function_xflex_shrink, std::move(child));
}

/// @brief Minimize if needed on the Y axis.
/// @ingroup dom
Element yflex_shrink(Element child) {
  return std::make_shared<Flex>(function_yflex_shrink, std::move(child));
}

/// @brief Make the element not flexible.
/// @ingroup dom
Element notflex(Element child) {
  return std::make_shared<Flex>(function_not_flex, std::move(child));
}

}  // namespace ftxui
