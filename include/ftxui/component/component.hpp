#ifndef FTXUI_COMPONENT_HPP
#define FTXUI_COMPONENT_HPP

#include <functional>  // for function
#include <memory>      // for make_shared, shared_ptr
#include <string>      // for wstring
#include <utility>     // for forward
#include <vector>      // for vector

#include "ftxui/component/component_base.hpp"     // for Component, Components
#include "ftxui/component/component_options.hpp"  // for ButtonOption, CheckboxOption, InputOption (ptr only), MenuEntryOption (ptr only), MenuOption, RadioboxOption (ptr only)
#include "ftxui/dom/elements.hpp"                 // for Element
#include "ftxui/util/ref.hpp"  // for Ref, ConstStringRef, ConstStringListRef, StringRef

namespace ftxui {
struct ButtonOption;
struct CheckboxOption;
struct Event;
struct InputOption;
struct MenuOption;
struct RadioboxOption;
struct MenuEntryOption;

template <class T, class... Args>
std::shared_ptr<T> Make(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

// Pipe operator to decorate components.
using ComponentDecorator = std::function<Component(Component)>;
using ElementDecorator = std::function<Element(Element)>;
Component operator|(Component component, ComponentDecorator decorator);
Component operator|(Component component, ElementDecorator decorator);
Component& operator|=(Component& component, ComponentDecorator decorator);
Component& operator|=(Component& component, ElementDecorator decorator);

namespace Container {
Component Vertical(Components children);
Component Vertical(Components children, int* selector);
Component Horizontal(Components children);
Component Horizontal(Components children, int* selector);
Component Tab(Components children, int* selector);

}  // namespace Container

Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 Ref<ButtonOption> = ButtonOption::Simple());

Component Checkbox(ConstStringRef label,
                   bool* checked,
                   Ref<CheckboxOption> option = CheckboxOption::Simple());

Component Input(StringRef content,
                ConstStringRef placeholder,
                Ref<InputOption> option = {});

Component Menu(ConstStringListRef entries,
               int* selected_,
               Ref<MenuOption> = MenuOption::Vertical());
Component MenuEntry(ConstStringRef label, Ref<MenuEntryOption> = {});

Component Dropdown(ConstStringListRef entries, int* selected);

Component Radiobox(ConstStringListRef entries,
                   int* selected_,
                   Ref<RadioboxOption> option = {});
Component Toggle(ConstStringListRef entries, int* selected);

template <class T>  // T = {int, float, long}
Component Slider(ConstStringRef label, T* value, T min, T max, T increment);

Component ResizableSplitLeft(Component main, Component back, int* main_size);
Component ResizableSplitRight(Component main, Component back, int* main_size);
Component ResizableSplitTop(Component main, Component back, int* main_size);
Component ResizableSplitBottom(Component main, Component back, int* main_size);

Component Renderer(Component child, std::function<Element()>);
Component Renderer(std::function<Element()>);
Component Renderer(std::function<Element(bool /* focused */)>);
ComponentDecorator Renderer(ElementDecorator);

Component CatchEvent(Component child, std::function<bool(Event)>);
ComponentDecorator CatchEvent(std::function<bool(Event)> on_event);

Component Maybe(Component, const bool* show);
Component Maybe(Component, std::function<bool()>);
ComponentDecorator Maybe(const bool* show);
ComponentDecorator Maybe(std::function<bool()>);

Component Collapsible(ConstStringRef label,
                      Component child,
                      Ref<bool> show = false);
}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
