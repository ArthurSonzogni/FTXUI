#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

std::unique_ptr<Node> hcenter(Element child) {
  return hbox(flex(), std::move(child), flex());
}

std::unique_ptr<Node> vcenter(Element child) {
  return vbox(flex(), std::move(child), flex());
}

std::unique_ptr<Node> center(Element child) {
  return hcenter(vcenter(std::move(child)));
}

} // namespace dom
} // namespace ftxui
