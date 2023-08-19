// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_CAPTURED_MOUSE_HPP
#define FTXUI_CAPTURED_MOUSE_HPP

#include <memory>

namespace ftxui {
class CapturedMouseInterface {
 public:
  virtual ~CapturedMouseInterface() = default;
};
using CapturedMouse = std::unique_ptr<CapturedMouseInterface>;
}  // namespace ftxui

#endif /* end of include guard: FTXUI_CAPTURED_MOUSE_HPP */
