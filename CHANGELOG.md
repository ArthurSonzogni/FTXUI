Changelog
=========

current (development) 
---------------------

### Component
- Feature: Add support for `Input`'s insert mode. Add `InputOption::insert`
  option. Added by @mingsheng13.

### Build
- Support for cmake's "unity/jumbo" builds. Fixed by @ClausKlein.

5.0.0
-----

### Component
- Breaking: MenuDirection enum is renamed Direction
- Breaking: GaugeDirection enum is renamed Direction
- Breaking: Direction enum is renamed WidthOrHeight
- Breaking: Remove `ComponentBase` copy constructor/assignment.
- Breaking: MenuOption::entries is renamed MenuOption::entries_option.
- Breaking: `Ref<{Component}Option>` becomes `{Component}Option` in component constructors.
- Feature: `ResizeableSplit` now support arbitrary element as a separator.
- Feature: `input` is now supporting multiple lines.
- Feature: `input` style is now customizeable.
- Bugfix: Support F1-F5 from OS terminal.
- Feature: Add struct based constructor:
  ```cpp
  Component Button(ButtonOption options);
  Component Checkbox(CheckboxOption options);
  Component Input(InputOption options);
  Component Menu(MenuOption options);
  Component MenuEntry(MenuEntryOption options);
  Component Radiobox(RadioboxOption options);
  Component Slider(SliderOption<T> options);
  Component ResizableSplit(ResizableSplitOption options);
  ```
- Feature: Add `ScreenInteractive::TrackMouse(false)` disable mouse support.

### Dom
- Feature: Add `hyperlink` decorator. For instance:
  ```cpp
  auto link = text("Click here") | hyperlink("https://github.com/FTXUI")
  ```
  See the [OSC 8 page](https://gist.github.com/egmontkob/eb114294efbcd5adb1944c9f3cb5feda).
  FTXUI support proposed by @aaleino in [#662](https://github.com/ArthurSonzogni/FTXUI/issues/662).

### Screen
- Breaking: `WordBreakProperty` becomes a uint8_t enum. This yields a 0.8%
  performance improvement.
- Breaking: Remove user defined Pixel constructor and equality operator.
- Performance: 19% faster on benchmarks.


### Build
- Check version compatibility when using cmake find_package()
- Add `FTXUI_DEV_WARNING` options to turn on warnings when building FTXUI
- Turn OFF by default `FTXUI_BUILD_DOCS`
- Turn OFF by default `FTXUI_BUILD_EXAMPLE`

4.1.1
-----

### Component
- Fix: Support arrow keys in application mode
- Fix: Remove useless new line when using an alternative screen.

### Dom
- Feature: Add the dashed style for border and separator:
  - See `DASHED` enum, and  `separatorDashed()`, `borderDashed()` functions.
- Feature: Add colored borders.
  - See functions: `borderStyled(BorderStyle, Color)` and `borderStyled(Color)`.
- Feature: Add `LinearGradient`. It can be used in `color` and `bgColor`.
- Improvement: Color::Interpolate() uses gamma correction.
- Fix: Check the `graph` area is positive.

### Build/Install
- Use globally set CMAKE_CXX_STANDARD if it is set.
- Expose the pkg-config file
- Check version compatibility when using cmake find_package()

4.1.0  (Abandonned)
-----
This version is abandonned and must not be used. It introduced a breaking change in the API.

4.0.0
-----

### DOM
- Feature: more styles:
  - `strikethrough`
  - `underlinedDouble`
- Feature: Customize the cursor. Add the following decorators:
  - `focusCursorBlock`
  - `focusCursorBlockBlinking`
  - `focusCursorBar`
  - `focusCursorBarBlinking`
  - `focusCursorUnderline`
  - `focusCursorUnderlineBlinking`
- Bugfix: Fix `focus`/`select` when the `vbox`/`hbox`/`dbox` contains a
  `flexbox`
- Bugfix: Fix the selected/focused area. It used to be 1 cell larger/longer than
  requested
- Bugfix: Forward the selected/focused area from the child in gridbox.
- Bugfix: Fix incorrect Canvas computed dimensions.
- Bugfix: Support `vscroll_indicator` with a zero inner size.
- Bugfix: Fix `vscroll_indicator` hidding the last column.

### Component:
- Feature: Add the `Modal` component.
- Feature: `Slider` supports taking references for all its arguments.
- Feature: `Slider` supports `SliderOption`. It supports:
    - multiple directions.
    - multiple colors.
    - various values (value, min, max, increment).
- Feature: Define `ScreenInteractive::Exit()`.
- Feature: Add `Loop` to give developers a better control on the main loop. This
  can be used to integrate FTXUI into another main loop, without taking the full
  control.
- Feature: `Input` supports CTRL+Left and CTRL+Right
- Feature: Use a blinking bar in the `Input` component.
- Improvement: The `Menu` keeps the focus when an entry is selected with the
  mouse.
- Bugfix: Add implementation of `ButtonOption::Border()`. It was missing.
- Bugfix: Provide the correct key for F1-F4 and F11.
- Feature: Add the `Hoverable` component decorators.

### Screen
- Feature: add `Box::Union(a,b) -> Box`
- Bugfix: Fix resetting `dim` clashing with resetting of `bold`.
- Feature: Add emscripten screen resize support.
- Bugfix: Add unicode 13 support for full width characters.
- Bugfix: Fix MSVC treating codecvt C++17 deprecated function as an error.

### Build
- Support using the google test version provided by the package manager.

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
