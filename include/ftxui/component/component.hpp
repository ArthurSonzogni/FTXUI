#ifndef FTXUI_COMPONENT_HPP
#define FTXUI_COMPONENT_HPP

#include <functional>  // for function
#include <memory>      // for shared_ptr, make_shared
#include <string>      // for wstring
#include <vector>      // for vector

#include "ftxui/component/component_base.hpp"

namespace ftxui {

class ComponentBase;

using Component = std::shared_ptr<ComponentBase>;
using Components = std::vector<Component>;

template <class T, class... Args>
std::shared_ptr<T> Make(Args&&... args) {
  return std::make_shared<T>(args...);
}

Component Button(const std::wstring* label, std::function<void()> on_click);
Component Checkbox(const std::wstring* label, bool* checked);
Component Input(std::wstring* content, const std::wstring* placeholder);
Component Menu(const std::vector<std::wstring>* entries, int* selected_);
Component Radiobox(const std::vector<std::wstring>* entries, int* selected_);
Component Toggle(const std::vector<std::wstring>* entries, int* selected);
Component Renderer(Component child, std::function<Element()>);
Component Renderer(std::function<Element()>);

template <class T>  // T = {int, float}
Component Slider(std::wstring label, T* value, T min, T max, T increment);

namespace Container {
Component Vertical(Components children);
Component Horizontal(Components children);
Component Tab(int* selector, Components children);
}  // namespace Container

};  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
