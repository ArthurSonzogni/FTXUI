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

/// Construct a Component of type T wrapped in a std::shared_ptr.
template <class T, class... Args>
std::shared_ptr<T> Make(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

// Pipe operator to decorate components.
using ComponentDecorator = std::function<Component(Component)>;
using ElementDecorator = std::function<Element(Element)>;

/// Apply a ComponentDecorator to a component.
FTXUI_EXPORT(COMPONENT)
Component operator|(Component component, ComponentDecorator decorator);
/// Apply an ElementDecorator to the rendered output of a component.
FTXUI_EXPORT(COMPONENT)
Component operator|(Component component, ElementDecorator decorator);
/// Apply a ComponentDecorator to a component in place.
FTXUI_EXPORT(COMPONENT)
Component& operator|=(Component& component, ComponentDecorator decorator);
/// Apply an ElementDecorator to a component in place.
FTXUI_EXPORT(COMPONENT)
Component& operator|=(Component& component, ElementDecorator decorator);

namespace Container {
/// Vertical list of components navigated with Up/Down and Tab.
FTXUI_EXPORT(COMPONENT) Component Vertical(Components children);
/// Vertical list of components with external selection index.
FTXUI_EXPORT(COMPONENT) Component Vertical(Components children, int* selector);
/// Horizontal list of components navigated with Left/Right and Tab.
FTXUI_EXPORT(COMPONENT) Component Horizontal(Components children);
/// Horizontal list of components with external selection index.
FTXUI_EXPORT(COMPONENT)
Component Horizontal(Components children, int* selector);
/// Display and route events only to the child at `*selector`.
FTXUI_EXPORT(COMPONENT) Component Tab(Components children, int* selector);
/// Stack components on top of each other (dbox-like); top child receives input.
FTXUI_EXPORT(COMPONENT) Component Stacked(Components children);
}  // namespace Container

/// Interactive button configured via ButtonOption.
FTXUI_EXPORT(COMPONENT) Component Button(ButtonOption options);
/// Interactive button invoking `on_click` when pressed or clicked.
FTXUI_EXPORT(COMPONENT)
Component Button(ConstStringRef label,
                 std::function<void()> on_click,
                 ButtonOption options = ButtonOption::Simple());

/// Interactive checkbox configured via CheckboxOption.
FTXUI_EXPORT(COMPONENT) Component Checkbox(CheckboxOption options);
/// Interactive checkbox toggling `*checked`.
FTXUI_EXPORT(COMPONENT)
Component Checkbox(ConstStringRef label,
                   bool* checked,
                   CheckboxOption options = CheckboxOption::Simple());

/// Editable text input field configured via InputOption.
FTXUI_EXPORT(COMPONENT) Component Input(InputOption options = {});
/// Editable text input bound to `content`.
FTXUI_EXPORT(COMPONENT)
Component Input(StringRef content, InputOption options = {});
/// Editable text input bound to `content` with a `placeholder`.
FTXUI_EXPORT(COMPONENT)
Component Input(StringRef content,
                StringRef placeholder,
                InputOption options = {});

/// Selectable list of items configured via MenuOption.
FTXUI_EXPORT(COMPONENT) Component Menu(MenuOption options);
/// Selectable list of `entries` updating `*selected_`.
FTXUI_EXPORT(COMPONENT)
Component Menu(ConstStringListRef entries,
               int* selected_,
               MenuOption options = MenuOption::Vertical());
/// Single selectable menu entry configured via MenuEntryOption.
FTXUI_EXPORT(COMPONENT) Component MenuEntry(MenuEntryOption options);
/// Single selectable menu entry with the given `label`.
FTXUI_EXPORT(COMPONENT)
Component MenuEntry(ConstStringRef label, MenuEntryOption options = {});

/// Radio button group configured via RadioboxOption.
FTXUI_EXPORT(COMPONENT) Component Radiobox(RadioboxOption options);
/// Radio button group for `entries` updating `*selected_`.
FTXUI_EXPORT(COMPONENT)
Component Radiobox(ConstStringListRef entries,
                   int* selected_,
                   RadioboxOption options = {});

/// Collapsible dropdown menu selecting one of `entries` into `*selected`.
FTXUI_EXPORT(COMPONENT)
Component Dropdown(ConstStringListRef entries, int* selected);
/// Collapsible dropdown menu configured via DropdownOption.
FTXUI_EXPORT(COMPONENT) Component Dropdown(DropdownOption options);

/// Horizontal toggle selector choosing one of `entries` into `*selected`.
FTXUI_EXPORT(COMPONENT)
Component Toggle(ConstStringListRef entries, int* selected);

/// Interactive numeric slider configured via SliderOption<T>.
template <typename T>
Component Slider(SliderOption<T> options);

extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int8_t>);
extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int16_t>);
extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int32_t>);
extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int64_t>);

extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<uint8_t>);
extern template FTXUI_EXPORT(COMPONENT) Component
    Slider(SliderOption<uint16_t>);
extern template FTXUI_EXPORT(COMPONENT) Component
    Slider(SliderOption<uint32_t>);
extern template FTXUI_EXPORT(COMPONENT) Component
    Slider(SliderOption<uint64_t>);

extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<float>);
extern template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<double>);

/// Labeled integer slider adjusting `value` in [`min`, `max`].
FTXUI_EXPORT(COMPONENT)
Component Slider(ConstStringRef label,
                 Ref<int> value,
                 ConstRef<int> min = 0,
                 ConstRef<int> max = 100,
                 ConstRef<int> increment = 5);
/// Labeled float slider adjusting `value` in [`min`, `max`].
FTXUI_EXPORT(COMPONENT)
Component Slider(ConstStringRef label,
                 Ref<float> value,
                 ConstRef<float> min = 0.f,
                 ConstRef<float> max = 100.f,
                 ConstRef<float> increment = 5.f);
/// Labeled long slider adjusting `value` in [`min`, `max`].
FTXUI_EXPORT(COMPONENT)
Component Slider(ConstStringRef label,
                 Ref<long> value,
                 ConstRef<long> min = 0L,
                 ConstRef<long> max = 100L,
                 ConstRef<long> increment = 5L);

/// Two-pane split view with a mouse-draggable separator.
FTXUI_EXPORT(COMPONENT)
Component ResizableSplit(ResizableSplitOption options);
/// Resizable split placing `main` on the left of `back`.
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitLeft(Component main, Component back, int* main_size);
/// Resizable split placing `main` on the right of `back`.
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitRight(Component main, Component back, int* main_size);
/// Resizable split placing `main` above `back`.
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitTop(Component main, Component back, int* main_size);
/// Resizable split placing `main` below `back`.
FTXUI_EXPORT(COMPONENT)
Component ResizableSplitBottom(Component main, Component back, int* main_size);

/// Wrap `child` with a custom render callback while forwarding events to it.
FTXUI_EXPORT(COMPONENT)
Component Renderer(Component child, std::function<Element()>);
/// Non-focusable component rendering the element returned by the callback.
FTXUI_EXPORT(COMPONENT) Component Renderer(std::function<Element()>);
/// Focusable component rendering an element based on its focus state.
FTXUI_EXPORT(COMPONENT)
Component Renderer(std::function<Element(bool /* focused */)>);
/// Decorator applying an ElementDecorator to a component's rendered output.
FTXUI_EXPORT(COMPONENT) ComponentDecorator Renderer(ElementDecorator);

/// Intercept events before `child`; return true from the handler to consume.
FTXUI_EXPORT(COMPONENT)
Component CatchEvent(Component child, std::function<bool(Event)>);
/// Decorator intercepting events before a component; return true to consume.
FTXUI_EXPORT(COMPONENT)
ComponentDecorator CatchEvent(std::function<bool(Event)> on_event);

/// Conditionally render and route events to a component when `*show` is true.
FTXUI_EXPORT(COMPONENT) Component Maybe(Component, const bool* show);
/// Conditionally render and route events to a component while predicate holds.
FTXUI_EXPORT(COMPONENT) Component Maybe(Component, std::function<bool()>);
/// Decorator showing a component only when `*show` is true.
FTXUI_EXPORT(COMPONENT) ComponentDecorator Maybe(const bool* show);
/// Decorator showing a component only while the predicate returns true.
FTXUI_EXPORT(COMPONENT) ComponentDecorator Maybe(std::function<bool()>);

/// Overlay `modal` on top of `main` when `*show_modal` is true.
FTXUI_EXPORT(COMPONENT)
Component Modal(Component main, Component modal, const bool* show_modal);
/// Decorator overlaying `modal` on top of a component when `*show_modal` holds.
FTXUI_EXPORT(COMPONENT)
ComponentDecorator Modal(Component modal, const bool* show_modal);

/// Expandable/collapsible section with a toggleable `label` and `child`.
FTXUI_EXPORT(COMPONENT)
Component Collapsible(ConstStringRef label,
                      Component child,
                      Ref<bool> show = false);

/// Track whether the mouse hovers over `component` in `*hover`.
FTXUI_EXPORT(COMPONENT)
Component Hoverable(Component component, bool* hover);
/// Invoke `on_enter` / `on_leave` when the mouse enters or leaves `component`.
FTXUI_EXPORT(COMPONENT)
Component Hoverable(Component component,
                    std::function<void()> on_enter,
                    std::function<void()> on_leave);
/// Invoke `on_change(hovered)` when the hover state of `component` changes.
FTXUI_EXPORT(COMPONENT)
Component Hoverable(Component component,  //
                    std::function<void(bool)> on_change);
/// Decorator tracking mouse hover state in `*hover`.
FTXUI_EXPORT(COMPONENT) ComponentDecorator Hoverable(bool* hover);
/// Decorator invoking `on_enter` / `on_leave` on mouse hover transitions.
FTXUI_EXPORT(COMPONENT)
ComponentDecorator Hoverable(std::function<void()> on_enter,
                             std::function<void()> on_leave);
/// Decorator invoking `on_change(hovered)` when mouse hover state changes.
FTXUI_EXPORT(COMPONENT)
ComponentDecorator Hoverable(std::function<void(bool)> on_change);

/// Draggable, resizable floating window configured via WindowOptions.
FTXUI_EXPORT(COMPONENT) Component Window(WindowOptions option);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_HPP */
