#ifndef FTXUI_COMPONENT_HPP
#define FTXUI_COMPONENT_HPP

#include <functional>  // for function
#include <memory>      // for make_shared, shared_ptr
#include <string>      // for wstring
#include <vector>      // for vector

#include "ftxui/component/component_base.hpp"     // for Component, Components
#include "ftxui/component/component_options.hpp"  // for ButtonOption, CheckboxOption, InputOption, MenuOption, RadioboxOption, ToggleOption
#include "ftxui/dom/elements.hpp"                 // for Element
#include "ftxui/util/ref.hpp"  // for Ref, ConstStringRef, StringRef

namespace ftxui {
struct ButtonOption;
struct CheckboxOption;
struct Event;
struct InputOption;
struct MenuOption;
struct RadioboxOption;
struct ToggleOption;

template <class T, class... Args>
std::shared_ptr<T> Make(Args&&... args) {
  return std::make_shared<T>(args...);
}

Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 Ref<ButtonOption> = {});
Component Checkbox(ConstStringRef label,
                   bool* checked,
                   Ref<CheckboxOption> option = {});
Component Input(StringRef content,
                ConstStringRef placeholder,
                Ref<InputOption> option = {});
Component Menu(const std::vector<std::wstring>* entries,
               int* selected_,
               Ref<MenuOption> = {});
Component Radiobox(const std::vector<std::wstring>* entries,
                   int* selected_,
                   Ref<RadioboxOption> option = {});
Component Toggle(const std::vector<std::wstring>* entries,
                 int* selected,
                 Ref<ToggleOption> option = {});
template <class T>  // T = {int, float, long}
Component Slider(StringRef label, T* value, T min, T max, T increment);
Component ResizableSplitLeft(Component main, Component back, int* main_size);
Component ResizableSplitRight(Component main, Component back, int* main_size);
Component ResizableSplitTop(Component main, Component back, int* main_size);
Component ResizableSplitBottom(Component main, Component back, int* main_size);
Component Renderer(Component child, std::function<Element()>);
Component Renderer(std::function<Element()>);
Component CatchEvent(Component child, std::function<bool(Event)>);

namespace Container {
Component Vertical(Components children);
Component Vertical(Components children, int* selector);
Component Horizontal(Components children);
Component Horizontal(Components children, int* selector);
Component Tab(Components children, int* selector);

}  // namespace Container

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
