#ifndef CORE_DOM_NODE_HPP
#define CORE_DOM_NODE_HPP

#include <memory>
#include <vector>

#include "ftxui/core/requirement.hpp"
#include "ftxui/core/screen.hpp"
#include "ftxui/core/box.hpp"

namespace ftxui {
namespace dom {

class Node {
 public:
  Node();
  Node(std::vector<std::unique_ptr<Node>> children);
  virtual ~Node();

  // Step 1: Direction parent <= children.
  //         Compute layout requirement. Tell parent what dimensions this
  //         element wants to be.
  virtual void ComputeRequirement();
  Requirement requirement() { return requirement_; }

  // Step 2: Direction parent => children.
  //         Assign this element its final dimensions.
  virtual void SetBox(Box box);

  // Step 3: Draw this element.
  virtual void Render(Screen& screen);

  std::vector<std::unique_ptr<Node>> children;
 protected:
  Requirement requirement_;
  Box box_;
};

void Render(Screen& screen, Node* node);

};  // namespace dom
};  // namespace ftxui

#endif /* end of include guard: CORE_DOM_NODE_HPP */
