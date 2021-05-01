#ifndef FTXUI_COMPONENT_SLIDER_HPP
#define FTXUI_COMPONENT_SLIDER_HPP

#include <string>
#include "ftxui/component/component.hpp"

namespace ftxui {
// ComponentPtr Slider(std::string label,
// float* value,
// float min = 0.f,
// float max = 100.f,
// float increment = (max - min) * 0.05f);

template <class T>  // T = {int, float}
ComponentPtr Slider(std::wstring label, T* value, T min, T max, T increment);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_SLIDER_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
