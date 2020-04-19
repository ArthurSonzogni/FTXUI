// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <sstream>

#include "ftxui/dom/elements.hpp"

namespace ftxui {

Elements paragraph(std::wstring the_text) {
  Elements output;
  std::wstringstream ss(the_text);
  std::wstring word;
  while (std::getline(ss, word, L' ')) {
    output.push_back(text(word + L' '));
  }
  return output;
}

}  // namespace ftxui
