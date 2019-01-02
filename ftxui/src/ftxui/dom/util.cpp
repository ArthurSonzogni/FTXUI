#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

Element nothing(Element element) {
  return std::move(element);
}

};  // namespace dom
};  // namespace ftxui
