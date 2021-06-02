#ifndef FTXUI_COMPONENT_HPP
#define FTXUI_COMPONENT_HPP

#include <functional>  // for function
#include <memory>      // for shared_ptr, make_shared
#include <string>      // for wstring
#include <vector>      // for vector

#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"   // for Element
#include "ftxui/screen/string.hpp"  // for ConstStringRef, StringRef

namespace ftxui {

class ComponentBase;
struct Event;

using Component = std::shared_ptr<ComponentBase>;
using Components = std::vector<Component>;

template <class T, class... Args>
std::shared_ptr<T> Make(Args&&... args) {
  return std::make_shared<T>(args...);
}

Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 bool border = true);
Component Checkbox(ConstStringRef label, bool* checked);
Component Input(StringRef content, ConstStringRef placeholder);
Component Menu(const std::vector<std::wstring>* entries, int* selected_);
Component Radiobox(const std::vector<std::wstring>* entries, int* selected_);
Component Toggle(const std::vector<std::wstring>* entries, int* selected);
template <class T>  // T = {int, float, long}
Component Slider(StringRef label, T* value, T min, T max, T increment);
Component Renderer(Component child, std::function<Element()>);
Component Renderer(std::function<Element()>);
Component CatchEvent(Component child, std::function<bool(Event)>);

namespace Container {
Component Vertical(Components children);
Component Horizontal(Components children);
Component Tab(Components children, int* selector);

}  // namespace Container

Component ResizableSplitLeft(Component main, Component back, int* main_size);
Component ResizableSplitRight(Component main, Component back, int* main_size);
Component ResizableSplitTop(Component main, Component back, int* main_size);
Component ResizableSplitBottom(Component main, Component back, int* main_size);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
