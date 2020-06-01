// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

Element hcenter(Element child) {
  return hbox(filler(), std::move(child), filler()) | flex_grow;
}

Element vcenter(Element child) {
  return vbox(filler(), std::move(child), filler()) | flex_grow;
}

Element center(Element child) {
  return hcenter(vcenter(std::move(child))) | flex_grow;
}

Element align_right(Element child) {
  return hbox(filler(), std::move(child)) | flex_grow;
}

}  // namespace ftxui
