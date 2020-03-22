#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

std::unique_ptr<Node> hcenter(Element child) {
  return hbox(filler(), std::move(child), filler());
}

std::unique_ptr<Node> vcenter(Element child) {
  return vbox(filler(), std::move(child), filler());
}

std::unique_ptr<Node> center(Element child) {
  return hcenter(vcenter(std::move(child)));
}

std::unique_ptr<Node> align_right(Element child) {
  return hbox(filler(), std::move(child));
}

}  // namespace ftxui
