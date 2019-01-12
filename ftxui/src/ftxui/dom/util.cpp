#include "ftxui/dom/elements.hpp"

namespace ftxui {

Element nothing(Element element) {
  return std::move(element);
}

Decorator compose(Decorator a, Decorator b) {
  return [
    a = std::move(a),
    b = std::move(b)
  ](Element element) {
    return b(a(std::move(element)));
  };
}

Decorator operator|(Decorator a, Decorator b) {
  return compose(a, b);
}

Element operator|(Element e, Decorator d) {
  return d(std::move(e));
}

}  // namespace ftxui
