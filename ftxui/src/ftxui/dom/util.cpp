#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

Element nothing(Element element) {
  return std::move(element);
}

Decorator compose(Decorator a, Decorator b) {
  return [
    a = std::move(a),
    b = std::move(b)
  ](Element element) {
    return a(b(std::move(element)));
  };
}

};  // namespace dom
};  // namespace ftxui
