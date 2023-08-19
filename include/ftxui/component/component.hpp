// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_HPP
#define FTXUI_COMPONENT_HPP

#include <functional>  // for function
#include <memory>      // for make_shared, shared_ptr
#include <string>      // for wstring
#include <utility>     // for forward
#include <vector>      // for vector

#include "ftxui/component/component_base.hpp"  // for Component, Components
#include "ftxui/component/component_options.hpp"  // for ButtonOption, CheckboxOption, MenuOption
#include "ftxui/dom/elements.hpp"  // for Element
#include "ftxui/util/ref.hpp"  // for ConstRef, Ref, ConstStringRef, ConstStringListRef, StringRef

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
Component Stacked(Components children);
}  // namespace Container

Component Button(ButtonOption options);
Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 ButtonOption options = ButtonOption::Simple());

Component Checkbox(CheckboxOption options);
Component Checkbox(ConstStringRef label,
                   bool* checked,
                   CheckboxOption options = CheckboxOption::Simple());

Component Input(InputOption options = {});
Component Input(StringRef content, InputOption options = {});
Component Input(StringRef content,
                StringRef placeholder,
                InputOption options = {});

Component Menu(MenuOption options);
Component Menu(ConstStringListRef entries,
               int* selected_,
               MenuOption options = MenuOption::Vertical());
Component MenuEntry(MenuEntryOption options);
Component MenuEntry(ConstStringRef label, MenuEntryOption options = {});

Component Radiobox(RadioboxOption options);
Component Radiobox(ConstStringListRef entries,
                   int* selected_,
                   RadioboxOption options = {});

Component Dropdown(ConstStringListRef entries, int* selected);
Component Toggle(ConstStringListRef entries, int* selected);

// General slider constructor:
template <typename T>
Component Slider(SliderOption<T> options);

// Shorthand without the `SliderOption` constructor:
Component Slider(ConstStringRef label,
                 Ref<int> value,
                 ConstRef<int> min = 0,
                 ConstRef<int> max = 100,
                 ConstRef<int> increment = 5);
Component Slider(ConstStringRef label,
                 Ref<float> value,
                 ConstRef<float> min = 0.f,
                 ConstRef<float> max = 100.f,
                 ConstRef<float> increment = 5.f);
Component Slider(ConstStringRef label,
                 Ref<long> value,
                 ConstRef<long> min = 0l,
                 ConstRef<long> max = 100l,
                 ConstRef<long> increment = 5l);

Component ResizableSplit(ResizableSplitOption options);
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

Component Modal(Component main, Component modal, const bool* show_modal);
ComponentDecorator Modal(Component modal, const bool* show_modal);

Component Collapsible(ConstStringRef label,
                      Component child,
                      Ref<bool> show = false);

Component Hoverable(Component component, bool* hover);
Component Hoverable(Component component,
                    std::function<void()> on_enter,
                    std::function<void()> on_leave);
Component Hoverable(Component component,  //
                    std::function<void(bool)> on_change);
ComponentDecorator Hoverable(bool* hover);
ComponentDecorator Hoverable(std::function<void()> on_enter,
                             std::function<void()> on_leave);
ComponentDecorator Hoverable(std::function<void(bool)> on_change);

Component Window(WindowOptions option);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */
