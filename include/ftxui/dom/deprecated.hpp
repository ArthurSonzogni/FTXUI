// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_DEPRECATED_HPP
#define FTXUI_DOM_DEPRECATED_HPP

#include <ftxui/dom/node.hpp>
#include <ftxui/util/export.hpp>
#include <string>

namespace ftxui {
FTXUI_EXPORT(DOM) Element text(std::wstring_view text);
FTXUI_EXPORT(DOM) Element vtext(std::wstring_view text);
}  // namespace ftxui

#endif  // FTXUI_DOM_DEPRECATED_HPP
