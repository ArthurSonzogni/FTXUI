#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

std::unique_ptr<Node> hcenter(Element child) {
  return hbox(filler(), std::move(child), filler());
}

std::unique_ptr<Node> vcenter(Element child) {
  return vbox(filler(), std::move(child), filler());
}

std::unique_ptr<Node> center(Element child) {
  return hcenter(vcenter(std::move(child)));
}

} // namespace dom
} // namespace ftxui
