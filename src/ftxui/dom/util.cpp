// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/dom/elements.hpp"

namespace ftxui {

Element nothing(Element element) {
  return element;
}

Decorator compose(Decorator a, Decorator b) {
  return [a = std::move(a), b = std::move(b)](Element element) {
    return b(a(std::move(element)));
  };
}

Decorator operator|(Decorator a, Decorator b) {
  return compose(a, b);
}

Elements operator|(Elements es, Decorator d) {
  Elements output;
  for (auto& it : es)
    output.push_back(std::move(it) | d);
  return output;
}

Element operator|(Element e, Decorator d) {
  return d(std::move(e));
}

}  // namespace ftxui
