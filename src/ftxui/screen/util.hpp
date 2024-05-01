// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_UTIL_HPP
#define FTXUI_SCREEN_UTIL_HPP

namespace ftxui::util {

// Similar to std::clamp, but allow hi to be lower than lo.
template <class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
  return v < lo ? lo : hi < v ? hi : v;
}

}  // namespace ftxui::util

#endif /* end of include guard: FTXUI_SCREEN_UTIL_HPP */
