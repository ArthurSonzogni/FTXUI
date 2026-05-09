// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_DEPRECATED_HPP
#define FTXUI_SCREEN_DEPRECATED_HPP

#include <string>

#include "ftxui/util/export.hpp"

namespace ftxui {
FTXUI_EXPORT(SCREEN) int wchar_width(wchar_t);
FTXUI_EXPORT(SCREEN) int wstring_width(const std::wstring&);
}  // namespace ftxui

#endif  // FTXUI_SCREEN_DEPRECATED_HPP
