// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_NODE_HPP
#define FTXUI_DOM_NODE_HPP

#include <memory>  // for shared_ptr
#include <vector>  // for vector

#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/dom/selection.hpp"    // for Selection
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"
#include "ftxui/util/export.hpp"

namespace ftxui {

class Node;
class Screen;

using Element = std::shared_ptr<Node>;
using Elements = std::vector<Element>;

/// @brief Node is the base class for all elements in the DOM tree.
///
/// It represents a single node in the document object model (DOM) and provides
/// the basic structure for layout and rendering.
/// It contains methods for computing layout requirements, setting the box
/// dimensions, selecting content, rendering to the screen, and checking the
/// layout status.
/// It typically contains child elements, which are also instances of Node.
///
/// Users are expected to derive from this class to create custom elements.
///
/// A list of builtin elements can be found in the `elements.hpp` file.
///
/// @ingroup dom
class FTXUI_EXPORT(DOM) Node {
 public:
  Node();
  explicit Node(Elements children);
  Node(const Node&) = delete;
  Node(const Node&&) = delete;
  Node& operator=(const Node&) = delete;
  Node& operator=(const Node&&) = delete;

  virtual ~Node();

  /// Step 1: Compute layout requirement (propagated from children to parents).
  virtual void ComputeRequirement();
  /// Return the computed layout requirement of this node.
  Requirement requirement() { return requirement_; }

  /// Step 2: Assign this element its final box (from parents to children).
  virtual void SetBox(Box box);

  /// Step 3 (optional): Propagate text selection from parents to children.
  virtual void Select(Selection& selection);

  /// Step 4: Draw this element onto `screen`.
  virtual void Render(Screen& screen);

  /// Return the text selected within this node.
  virtual std::string GetSelectedContent(Selection& selection);

  /// Layout iteration status used when multiple layout passes are needed.
  struct Status {
    int iteration = 0;
    bool need_iteration = false;
  };
  /// Request additional layout iterations if layout has not converged yet.
  virtual void Check(Status* status);

  // ABI Reserve:
  virtual void Reserved1();
  virtual void Reserved2();
  virtual void Reserved3();
  virtual void Reserved4();
  virtual void Reserved5();
  virtual void Reserved6();
  virtual void Reserved7();
  virtual void Reserved8();

  friend FTXUI_EXPORT(DOM) void Render(Screen& screen, Node* node, Selection& selection);

 protected:
  Elements children_;
  Requirement requirement_;
  Box box_;
};

/// Layout and render `element` into `screen`.
FTXUI_EXPORT(DOM) void Render(Screen& screen, const Element& element);
/// Layout and render `node` into `screen`.
FTXUI_EXPORT(DOM) void Render(Screen& screen, Node* node);
/// Layout, select, and render `node` into `screen` with `selection`.
FTXUI_EXPORT(DOM)
void Render(Screen& screen, Node* node, Selection& selection);
/// Layout `node` and extract the selected text without rendering.
FTXUI_EXPORT(DOM)
std::string GetNodeSelectedContent(Screen& screen,
                                   Node* node,
                                   Selection& selection);

}  // namespace ftxui

#endif  // FTXUI_DOM_NODE_HPP
