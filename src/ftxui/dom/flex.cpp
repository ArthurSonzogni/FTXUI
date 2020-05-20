// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

class Flex : public Node {
 public:
  Flex() {}
  Flex(Element child) : Node(unpack(std::move(child))) {}
  ~Flex() override {}
  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    if (!children.empty()) {
      children[0]->ComputeRequirement();
      requirement_ = children[0]->requirement();
    }
    requirement_.flex.x = 1;
    requirement_.flex.y = 1;
  }

  void SetBox(Box box) override {
    if (children.empty())
      return;
    children[0]->SetBox(box);
  }
};

class NotFlex : public Flex {
 public:
  NotFlex() {}
  NotFlex(Element child) : Flex(std::move(child)) {}
  ~NotFlex() override {}
  void ComputeRequirement() override {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    if (!children.empty()) {
      children[0]->ComputeRequirement();
      requirement_ = children[0]->requirement();
    }
    requirement_.flex.x = 0;
    requirement_.flex.y = 0;
  }
};

Element filler() {
  return std::make_shared<Flex>();
}

Element flex(Element child) {
  return std::make_shared<Flex>(std::move(child));
}

Element notflex(Element child) {
  return std::make_shared<NotFlex>(std::move(child));
}

}  // namespace ftxui
