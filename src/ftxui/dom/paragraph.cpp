// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <sstream>  // for basic_istream, stringstream
#include <string>   // for string, allocator, getline
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"  // for flexbox, Element, text, Elements, operator|, xflex, paragraph, paragraphAlignCenter, paragraphAlignJustify, paragraphAlignLeft, paragraphAlignRight
#include "ftxui/dom/flexbox_config.hpp"  // for FlexboxConfig, FlexboxConfig::JustifyContent, FlexboxConfig::JustifyContent::Center, FlexboxConfig::JustifyContent::FlexEnd, FlexboxConfig::JustifyContent::SpaceBetween

namespace ftxui {

namespace {
Elements Split(const std::string& the_text) {
  Elements output;
  std::stringstream ss(the_text);
  std::string word;
  while (std::getline(ss, word, ' ')) {
    output.push_back(text(word));
  }
  return output;
}
}  // namespace

/// @brief Return an element drawing the paragraph on multiple lines.
/// @ingroup dom
/// @see flexbox.
Element paragraph(const std::string& the_text) {
  return paragraphAlignLeft(the_text);
}

/// @brief Return an element drawing the paragraph on multiple lines, aligned on
/// the left.
/// @ingroup dom
/// @see flexbox.
Element paragraphAlignLeft(const std::string& the_text) {
  static const auto config = FlexboxConfig().SetGap(1, 0);
  return flexbox(Split(the_text), config);
}

/// @brief Return an element drawing the paragraph on multiple lines, aligned on
/// the right.
/// @ingroup dom
/// @see flexbox.
Element paragraphAlignRight(const std::string& the_text) {
  static const auto config =
      FlexboxConfig().SetGap(1, 0).Set(FlexboxConfig::JustifyContent::FlexEnd);
  return flexbox(Split(the_text), config);
}

/// @brief Return an element drawing the paragraph on multiple lines, aligned on
/// the center.
/// @ingroup dom
/// @see flexbox.
Element paragraphAlignCenter(const std::string& the_text) {
  static const auto config =
      FlexboxConfig().SetGap(1, 0).Set(FlexboxConfig::JustifyContent::Center);
  return flexbox(Split(the_text), config);
}

/// @brief Return an element drawing the paragraph on multiple lines, aligned
/// using a justified alignment.
/// the center.
/// @ingroup dom
/// @see flexbox.
Element paragraphAlignJustify(const std::string& the_text) {
  static const auto config = FlexboxConfig().SetGap(1, 0).Set(
      FlexboxConfig::JustifyContent::SpaceBetween);
  Elements words = Split(the_text);
  words.push_back(text("") | xflex);
  return flexbox(std::move(words), config);
}

}  // namespace ftxui
