#ifndef FTXUI_DOM_NODE_DECORATOR_H_
#define FTXUI_DOM_NODE_DECORATOR_H_

#include "ftxui/dom/node.hpp"

namespace ftxui {

// Helper class.
class NodeDecorator : public Node {
 public:
  NodeDecorator(Elements children) : Node(std::move(children)) {}
  ~NodeDecorator() override {}
  void ComputeRequirement() override;
  void SetBox(Box box) override;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_NODE_DECORATOR_H_ */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
