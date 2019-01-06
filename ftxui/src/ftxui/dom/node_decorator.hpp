#ifndef FTXUI_DOM_NODE_DECORATOR_H_
#define FTXUI_DOM_NODE_DECORATOR_H_

#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui::dom {

// Helper class.
class NodeDecorator : public Node {
 public:
  NodeDecorator(Children children) : Node(std::move(children)) {}
  ~NodeDecorator() override {}
  void ComputeRequirement() override;
  void SetBox(Box box) override;
};

};  // namespace ftxui::dom

#endif /* end of include guard: FTXUI_DOM_NODE_DECORATOR_H_ */
