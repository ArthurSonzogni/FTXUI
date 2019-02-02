#ifndef FTXUI_DOM_NODE_HPP
#define FTXUI_DOM_NODE_HPP

#include <memory>
#include <vector>

#include "ftxui/dom/requirement.hpp"
#include "ftxui/screen/box.hpp"
#include "ftxui/screen/screen.hpp"

namespace ftxui {

class Node {
 public:
  Node();
  Node(std::vector<std::unique_ptr<Node>> children);
  virtual ~Node();

  // Step 1: Compute layout requirement. Tell parent what dimensions this
  //         element wants to be.
  //         Propagated from Children to Parents.
  virtual void ComputeRequirement();
  Requirement requirement() { return requirement_; }

  // Step 2: Assign this element its final dimensions.
  //         Propagated from Parents to Children.
  virtual void SetBox(Box box);

  // Step 3: Draw this element.
  virtual void Render(Screen& screen);

  std::vector<std::unique_ptr<Node>> children;
 protected:
  Requirement requirement_;
  Box box_;
};

void Render(Screen& screen, Node* node);

};  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_NODE_HPP */
