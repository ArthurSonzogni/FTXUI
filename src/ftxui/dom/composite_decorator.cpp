#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"

namespace ftxui {

/// @brief Center an element horizontally.
/// @param The input element.
/// @return The centered element.
/// @ingroup dom
Element hcenter(Element child) {
  return hbox(filler(), std::move(child), filler()) | xflex_grow;
}

/// @brief Center an element vertically.
/// @param The input element.
/// @return The centered element.
/// @ingroup dom
Element vcenter(Element child) {
  return vbox(filler(), std::move(child), filler()) | yflex_grow;
}

/// @brief Center an element horizontally and vertically.
/// @param The input element.
/// @return The centered element.
/// @ingroup dom
Element center(Element child) {
  return hcenter(vcenter(std::move(child))) | flex_grow;
}

/// @brief Align an element on the right side.
/// @param The input element.
/// @return The right aligned element.
/// @ingroup dom
Element align_right(Element child) {
  return hbox(filler(), std::move(child)) | flex_grow;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
