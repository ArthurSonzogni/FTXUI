// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_DEPRECATED_HPP
#define FTXUI_DOM_DEPRECATED_HPP

#include <ftxui/dom/node.hpp>
#include <string>

namespace ftxui {
Element text(std::wstring text);
Element vtext(std::wstring text);
Elements paragraph(std::wstring text);
}  // namespace ftxui

#endif  // FTXUI_DOM_DEPRECATED_HPP
