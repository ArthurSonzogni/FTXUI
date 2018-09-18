#include "ftxui/core/dom/node.hpp"

namespace ftxui {
namespace dom {

class Flex : public Node {
 public:
  Flex() {}
  ~Flex() override {}
  void ComputeRequirement() {
    requirement_.min.x = 0;
    requirement_.min.y = 0;
    requirement_.flex.x = 1;
    requirement_.flex.y = 1;
  }
};

std::unique_ptr<Node> flex() {
  return std::make_unique<Flex>();
}

};  // namespace dom
};  // namespace ftxui
