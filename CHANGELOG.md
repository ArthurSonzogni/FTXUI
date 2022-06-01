Changelog
=========

current (development) 
---------------------

### DOM
- Bugfix: Fix `focus`/`select` when the `vbox`/`hbox`/`dbox` contains a
  `flexbox`
- Bugfix: Fix the selected/focused area. It used to be 1 cell larger/longer than
  requested
- Bugfix: Forward the selected/focused area from the child in gridbox.
- Bugfix: Fix incorrect Canvas computed dimensions.

### Screen
- Feature: add `Box::Union(a,b) -> Box`

3.0.0
-----

### Build
- **breaking**: The library prefix is now back to "lib" (the default). This
    means non-cmake users should not link against "libftxui-dom" for instance.

### Component
- **Animations** module! Components can implement the `OnAnimation` method and
  the animation::Animator to define some animated properties.
  - `Menu` now support animations.
  - `Button` now supports animations.
- Support SIGTSTP. (ctrl+z).
- Support task posting. `ScreenInteractive::Post(Task)`.
- `Menu` can now be used in the 4 directions, using `MenuOption.direction`.
- `Menu` can display an animated underline, using
  `MenuOption.underline.enabled`.
- `Button` is now taking the focus in frame.
- **breaking** All the options are now using a transform function.
- **breaking** The `Toggle` component is now implemented using `Menu`.
- **bugfix** Container::Tab implements `Focusable()`.
- **bugfix** Improved default implementations of ComponentBase `Focusable()` and
  `ActiveChild()` methods.
- **bugfix** Automatically convert '\r' keys into '\n' for Linux programs that
  do not send the correct code for the return key, like the 'bind'.
  https://github.com/ArthurSonzogni/FTXUI/issues/337
- Add decorator for components:
  - `operator|(Component, ComponentDecorator)`
  - `operator|(Component, ElementDecorator)`
  - `operator|=(Component, ComponentDecorator)`
  - `operator|=(Component, ElementDecorator)`
  - Add the `Maybe` decorator.
  - Add the `CatchEvent` decorator.
  - Add the `Renderer` decorator.
- **breaking** remove the "deprectated.hpp" header and Input support for wide
    string.

### DOM:
- **breaking**: The `inverted` decorator now toggle in the inverted attribute.
- Add `gauge` for the 4 directions. Expose the following API:
```cpp
Element gauge(float ratio);
Element gaugeLeft(float ratio);
Element gaugeRight(float ratio);
Element gaugeUp(float ratio);
Element gaugeDown(float ratio);
Element gaugeDirection(float ratio, GaugeDirection);
```
- Add `separatorHSelector` and `separatorVSelector` elements. This can be used
  to highlight an area.
- Add the `automerge` decorator. This makes separator characters to be merged
  with others nearby.
- Fix the `Table` rendering function, to allow automerging characters.
- **Bugfix**: The `vscroll_indicator` now computes its offset and size
  correctly.
- Add the `operator|=(Element, Decorator)`

### Screen:
- Add: `Color::Interpolate(lambda, color_a, color_b)`.

2.0.0
-----

### Features:

#### Screen
- Add the `automerge` to the Pixel bit field. This now controls which pixels are
  automatically merged.

#### DOM:
- Add the `Canvas` class and `ElementFrom('canvas')` function. Together users of
  the library can draw using braille and block characters.
- Support `flexbox` dom elements. This is build symmetrically to the HTML one.
  All the following attributes are supported: direction, wrap, justify-content,
  align-items, align-content, gap
- Add the dom elements helper based on `flexbox`:
  - `paragraph`
  - `paragraphAlignLeft`
  - `paragraphAlignCenter`
  - `paragraphAlignRight`
  - `paragraphAlignJustify`
- Add the helper elements based on `flexbox`: `hflow()`, `vflow()`.
- Add: `focusPositionRelative` and `focusPosition`
- Add `Table` constructor from 2D vector of Element, instead of string.

#### Component 
- Add the `collapsible` component.
- Add the `ScreenInteractive::WithRestoredIO`. This decorates a callback. This
  runs it with the terminal hooks temporarilly uninstalled. This is useful if
  you want to execute command using directly stdin/stdout/sterr.

### Bug

#### Table
- The `table` horizontal and vertical separator are now correctly expanded.

#### Component 
- `Input` shouldn't take focus when hovered by the mouse.
- Modifying `Input`'s during on_enter/on_change event is now working correctly.

### Breaking changes:
- The behavior of `paragraph` has been modified. It now returns en Element,
  instead of a list of elements.

0.11.1
------

# Component
- Feature: Support for PageUp/PageDown/Home/End buttons.
- Bugfix: Check the selected element are within bounds for Dropdown.

# Build
- Bugfix: Package library using the "Release config". Not debug.

0.11
----

## github workflow
- Add Windows ad MacOS artefacts.
- Merge all the workflows.

## Bug
- On Unix system, fallback to {80,25} screen dimension on failure.

## CMake
- Support for shared library, via `BUILD_SHARED_LIBS` option.
- Add library version and symlinks.

0.10 (2021-09-30)
--------------------

## Bug
- Fix the automated merge of borders.

### Dom
- `Table()` class to build stylised table.
   See https://github.com/ArthurSonzogni/FTXUI/discussions/228
- `vscroll_indicator`. Show a scrollbar indicator on the right.
- `separatorEmpty`. A separator drawing nothing.
- `separatorFixed`. A separator drawing the provided character.

### Component
- `Maybe`: Display an component conditionnally based on a boolean.
- `Dropdown`: A dropdown select list.

0.9 (2021-09-26)
----------------

The initial release where changelog where written.

This version includes:

### screen
- Style:
  - Bold.
  - Blink.
  - Dim.
  - Inverted.
  - Underlined.
  - Foreground color.
  - Background color.
- Support for UTF8 unicode.
  - Full wide character: 测试.
  - Combining characters: a⃒
- A Stencil buffer.
- Automatically merge box drawing characters.
- Detect terminal dimension.

### DOM

- Element:
  - `text` & `vtext`
  - `separator` and 5 variations.
  - `gauge`
  - `border` and 6 variations.
  - `window`
  - `spinner`
  - `paragraph` and `hflow`.

- Layout:
  - `hbox`
  - `vbox`
  - `dbox`
  - `gridbox`
  - `frame`: Drawing inside a virtual area, potentially larger than the real
             one.
  - `focus`, `select`: scroll the inner view of a frame, to be in view.
  - `flex` & 8 variations. `filler`
  
- Decorators:
  - `bold`
  - `dim`
  - `inverted`
  - `blink`
  - `color`
  - `bgcolor`
  - `clearunder`

### Component

- Container:
  - `Container::Vertical`
  - `Container::Horizontal`
  - `Container::Tab`
- `Button`
- `Checkbox`
- `Input`
- `Menu`
- `MenuEntry`
- `Radiobox`
- `Toggle`
- `Slider`
- `Renderer` & variations
- `CatchEvent`

### MISC

- Fuzzer
- Tests using gtest.
- Doxygen documentation
- IWYU
- 52 examples.
- Support for WebAssembly.
- Support for Window and fallback for broken terminal.
