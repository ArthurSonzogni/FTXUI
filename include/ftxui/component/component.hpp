// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_HPP
#define FTXUI_COMPONENT_HPP

#include <functional>  // for function
#include <memory>      // for make_shared, shared_ptr
#include <utility>     // for forward

#include <ftxui/util/warn_windows_macro.hpp>
#include "ftxui/component/component_base.hpp"  // for Component, Components
#include "ftxui/component/component_options.hpp"  // for ButtonOption, CheckboxOption, MenuOption
#include "ftxui/dom/elements.hpp"  // for Element
#include "ftxui/util/export.hpp"
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
FTXUI_EXPORT(COMPONENT)
Component operator|(Component component, ComponentDecorator decorator);
FTXUI_EXPORT(COMPONENT)
Component operator|(Component component, ElementDecorator decorator);
FTXUI_EXPORT(COMPONENT)
Component& operator|=(Component& component, ComponentDecorator decorator);
FTXUI_EXPORT(COMPONENT)
Component& operator|=(Component& component, ElementDecorator decorator);

namespace Container {
FTXUI_EXPORT(COMPONENT) Component Vertical(Components children);
FTXUI_EXPORT(COMPONENT) Component Vertical(Components children, int* selector);
FTXUI_EXPORT(COMPONENT) Component Horizontal(Components children);
FTXUI_EXPORT(COMPONENT) Component Horizontal(Components children, int* selector);
FTXUI_EXPORT(COMPONENT) Component Tab(Components children, int* selector);
FTXUI_EXPORT(COMPONENT) Component Stacked(Components children);
}  // namespace Container

FTXUI_EXPORT(COMPONENT) Component Button(ButtonOption options);
FTXUI_EXPORT(COMPONENT)
Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 ButtonOption options = ButtonOption::Simple());

FTXUI_EXPORT(COMPONENT) Component Checkbox(CheckboxOption options);
FTXUI_EXPORT(COMPONENT)
Component Checkbox(ConstStringRef label,
                   bool* checked,
                   CheckboxOption options = CheckboxOption::Simple());

FTXUI_EXPORT(COMPONENT) Component Input(InputOption options = {});
FTXUI_EXPORT(COMPONENT) Component Input(StringRef content, InputOption options = {});
FTXUI_EXPORT(COMPONENT)
Component Input(StringRef content,
                StringRef placeholder,
                InputOption options = {});

FTXUI_EXPORT(COMPONENT) Component Menu(MenuOption options);
FTXUI_EXPORT(COMPONENT)
Component Menu(ConstStringListRef entries,
               int* selected_,
               MenuOption options = MenuOption::Vertical());
FTXUI_EXPORT(COMPONENT) Component MenuEntry(MenuEntryOption options);
FTXUI_EXPORT(COMPONENT)
Component MenuEntry(ConstStringRef label, MenuEntryOption options = {});

FTXUI_EXPORT(COMPONENT) Component Radiobox(RadioboxOption options);
FTXUI_EXPORT(COMPONENT)
Component Radiobox(ConstStringListRef entries,
                   int* selected_,
                   RadioboxOption options = {});

FTXUI_EXPORT(COMPONENT)
Component Dropdown(ConstStringListRef entries, int* selected);
FTXUI_EXPORT(COMPONENT) Component Dropdown(DropdownOption options);

FTXUI_EXPORT(COMPONENT)
Component Toggle(ConstStringListRef entries, int* selected);

// General slider constructor:
template <typename T>
Component Slider(SliderOption<T> options);

extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int8_t>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<int16_t>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<int32_t>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<int64_t>);

extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<uint8_t>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<uint16_t>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<uint32_t>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<uint64_t>);

extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<float>);
extern template FTXUI_EXPORT(COMPONENT)
    Component Slider(SliderOption<double>);

// Shorthand without the `SliderOption` constructor:
FTXUI_EXPORT(COMPONENT)
Component Slider(ConstStringRef label,
                 Ref<int> value,
                 ConstRef<int> min = 0,
                 ConstRef<int> max = 100,
                 ConstRef<int> increment = 5);
FTXUI_EXPORT(COMPONENT)
Component Slider(ConstStringRef label,
                 Ref<float> value,
                 ConstRef<float> min = 0.f,
                 ConstRef<float> max = 100.f,
                 ConstRef<float> increment = 5.f);
FTXUI_EXPORT(COMPONENT)
Component Slider(ConstStringRef label,
                 Ref<long> value,
                 ConstRef<long> min = 0L,
                 ConstRef<long> max = 100L,
                 ConstRef<long> increment = 5L);

FTXUI_EXPORT(COMPONENT)
Component ResizableSplit(ResizableSplitOption options);
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitLeft(Component main, Component back, int* main_size);
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitRight(Component main, Component back, int* main_size);
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitTop(Component main, Component back, int* main_size);
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitBottom(Component main, Component back, int* main_size);

FTXUI_EXPORT(COMPONENT)
Component Renderer(Component child, std::function<Element()>);
FTXUI_EXPORT(COMPONENT) Component Renderer(std::function<Element()>);
FTXUI_EXPORT(COMPONENT)
Component Renderer(std::function<Element(bool /* focused */)>);
FTXUI_EXPORT(COMPONENT) ComponentDecorator Renderer(ElementDecorator);

FTXUI_EXPORT(COMPONENT)
Component CatchEvent(Component child, std::function<bool(Event)>);
FTXUI_EXPORT(COMPONENT)
ComponentDecorator CatchEvent(std::function<bool(Event)> on_event);

FTXUI_EXPORT(COMPONENT) Component Maybe(Component, const bool* show);
FTXUI_EXPORT(COMPONENT) Component Maybe(Component, std::function<bool()>);
FTXUI_EXPORT(COMPONENT) ComponentDecorator Maybe(const bool* show);
FTXUI_EXPORT(COMPONENT) ComponentDecorator Maybe(std::function<bool()>);

FTXUI_EXPORT(COMPONENT)
Component Modal(Component main, Component modal, const bool* show_modal);
FTXUI_EXPORT(COMPONENT)
ComponentDecorator Modal(Component modal, const bool* show_modal);

FTXUI_EXPORT(COMPONENT)
Component Collapsible(ConstStringRef label,
                      Component child,
                      Ref<bool> show = false);

FTXUI_EXPORT(COMPONENT)
Component Hoverable(Component component, bool* hover);
FTXUI_EXPORT(COMPONENT)
Component Hoverable(Component component,
                    std::function<void()> on_enter,
                    std::function<void()> on_leave);
FTXUI_EXPORT(COMPONENT)
Component Hoverable(Component component,  //
                    std::function<void(bool)> on_change);
FTXUI_EXPORT(COMPONENT) ComponentDecorator Hoverable(bool* hover);
FTXUI_EXPORT(COMPONENT)
ComponentDecorator Hoverable(std::function<void()> on_enter,
                             std::function<void()> on_leave);
FTXUI_EXPORT(COMPONENT)
ComponentDecorator Hoverable(std::function<void(bool)> on_change);

FTXUI_EXPORT(COMPONENT) Component Window(WindowOptions option);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */
