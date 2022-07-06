
# Table of contents

- [Introduction](#introduction)
- [Build](#build)
    - [Using CMake](#using-cmake)
- [List of modules](#list-of-modules)
- [screen](#screen)
- [dom](#dom)
    - [text](#text)
    - [vtext](#vtext)
    - [paragraph](#paragraph)
    - [border](#border)
    - [window](#window)
    - [separator](#separator)
    - [gauge](#gauge)
    - [graph](#graph)
    - [colors](#colors)
    - [style](#style)
    - [layout](#layout)
    - [table](#table)
    - [canvas](#canvas)
- [component](#component)
    - [Input](#input)
    - [Menu](#menu)
    - [Toggle](#toggle)
    - [Checkbox](#checkbox)
    - [RadioBox](#radiobox)
    - [Dropdown](#dropdown)
    - [Slider](#slider)
    - [Renderer](#renderer)
    - [CatchEvent](#catchevent)
    - [Collapsible](#collapsible)
    - [Maybe](#maybe)
    - [Container](#container)
    - [ResizableSplit](#resizablesplit)
    - [Force a frame redraw](#force-a-frame-redraw)

# Introduction

Welcome to the FTXUI documentation!

This is a brief tutorial. You are also encouraged to self-learn by reading the [examples](./examples.html)

**Short example**

To build a single frame, you need create an `ftxui::Element`, and display it on
a `ftxui::Screen`.

**main.cpp**
```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main(void) {
  using namespace ftxui;

  // Define the document
  Element document =
    hbox({
      text("left")   | border,
      text("middle") | border | flex,
      text("right")  | border,
    });

  auto screen = Screen::Create(
    Dimension::Full(),       // Width
    Dimension::Fit(document) // Height
  );
  Render(screen, document);
  screen.Print();

  return EXIT_SUCCESS;
}
```

**output**
```bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
```

# Build

## Using CMake

This is an example configuration for your **CMakeLists.txt**

~~~cmake
cmake_minimum_required (VERSION 3.11)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  # Important: Specify a GIT_TAG XXXXX here.
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

project(ftxui-starter
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(ftxui-starter src/main.cpp)
target_include_directories(ftxui-starter PRIVATE src)

target_link_libraries(ftxui-starter
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)

~~~

Subsequently, you build the project in the standard fashion as follows:

~~~sh
mkdir build && cd build
cmake ..
make
./main
~~~

# List of modules

The project is comprised of 3 modules:

1. [**ftxui/screen**](#screen) defines a `ftxui::Screen`, a grid of `ftxui::Pixel`.

2. [**ftxui/dom**](#dom) is the main module. It defines a hierarchical set of
   `ftxui::Element`.  An element draws something on the `ftxui::Screen`. It is
   responsive to the size of its container.

3. [**ftxui/component**](#component) This modules is only required if your program needs to respond to user input. It defines a set of `ftxui::Component`.  These components can be utilized to navigate the program by using the arrow keys and interacting with widgets such as the checkbox. You can also make you own components.

# screen

This is the visual element of the program. It defines a `ftxui::Screen`, which is a grid of `ftxui::Pixel`. A Pixel represents a Unicode character and its associated style (bold, colors, etc.). The screen can be printed as a string using `ftxui::Screen::ToString()`. The following example highlights this process:

~~~cpp
  #include <ftxui/screen/screen.hpp>
  #include <iostream>

  int main(void) {
    using namespace ftxui;
    auto screen = Screen::Create(Dimension::Fixed(32), Dimension::Fixed(10));

    auto& pixel = screen.PixelAt(9,9);
    pixel.character = U'A';
    pixel.bold = true;
    pixel.foreground_color = Color::Blue;

    std::cout << screen.ToString();
    return EXIT_SUCCESS;
  }
~~~

# dom

This module defines a hierarchical set of `ftxui::Element`. An element manages
the layout and can be responsive to the terminal dimension changes. Note the following example where this module is used to create a simple layout with a number of operators:

**Example:**
```cpp
// Define the document
Element document = vbox({
  text("The window") | bold | color(Color::Blue),
  gauge(0.5)
  text("The footer")
});

// Add a border, by calling the `ftxui::border` decorator function.
document = border(document);

// Add another border, using the pipe operator.
document = document | border.

// Add another border, using the |= operator.
document |= border

```

**List of elements**

The list of all elements are included and can be accessed by including the corresponding header file:

```cpp
#include <ftxui/dom/elements.hpp>
```

## text

The most simple widget. It displays a text.

Code:
~~~cpp
text("I am a piece of text");
~~~
Terminal output:
~~~bash
I am a piece of text.
~~~

## vtext

Identical to `ftxui::text`, but displayed vertically.

Code:
~~~cpp
vtext("HELLO");
~~~
Terminal Output:
~~~bash
H
E
L
L
O
~~~

## paragraph

Similar to `ftxui::text`, but it supports line wrapping and alignments. The
words are split by spaces.

Sample Code:
```cpp
paragraph("A very long text")
```

Sample Terminal Output:

![ezgif com-gif-maker (4)](https://user-images.githubusercontent.com/4759106/147251370-983a06e7-6f41-4113-92b8-942f43d34d06.gif)

For a more detailed example refer to [detailed example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2paragraph_8cpp-example.html). Paragraph also includes a number of other variants as shown below:
```cpp
Element paragraph(std::string text);
Element paragraphAlignLeft(std::string text);
Element paragraphAlignRight(std::string text);
Element paragraphAlignCenter(std::string text);
Element paragraphAlignJustify(std::string text);
```

## border

Adds a border around an element.

Code:
~~~cpp
border(text("The element"))
~~~
Terminal Output:
~~~bash
┌───────────┐
│The element│
└───────────┘
~~~

You can achieve the same behavior by using the pipe operator.

Code:
```cpp
text("The element") | border
```

Border also comes in a variety of styles as shown below:
```cpp
Element border(Element);
Element borderLight(Element);
Element borderHeavy(Element);
Element borderDouble(Element);
Element borderRounded(Element);
Element borderEmpty(Element);
Decorator borderStyled(BorderStyle);
Decorator borderWith(Pixel);
```


## window

A `ftxui::window` is a `ftxui::border`, but with an additional header. To add a window around an element, wrap it and specify a string as the header.

Code:
~~~cpp
window("The window", text("The element"))
~~~
Terminal Output:
~~~bash
┌The window─┐
│The element│
└───────────┘
~~~

## separator

Displays a vertical/horizontal line to visually split the content of a
container in two.

Code:
~~~cpp
border(
  hbox({
    text("Left"), 
    separator(),
    text("Right")
  })
)
~~~
Terminal Output:
~~~bash
┌────┬─────┐
│left│right│
└────┴─────┘
~~~

Separators come in a variety of flavors as shown below:
```cpp
Element separator(void);
Element separatorLight();
Element separatorHeavy();
Element separatorDouble();
Element separatorEmpty();
Element separatorStyled(BorderStyle);
Element separator(Pixel);
Element separatorCharacter(std::string);
Element separatorHSelector(float left,
                           float right,
                           Color background,
                           Color foreground);
Element separatorVSelector(float up,
                           float down,
                           Color background,
                           Color foreground);
```

## gauge

It constitutes a gauge. It can be used to represent a progress bar.

Code:
~~~cpp
border(gauge(0.5))
~~~
Terminal Output:
~~~bash
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

Gauges can be displayed in many orientations as shown below:
```cpp
Element gauge(float ratio);
Element gaugeLeft(float ratio);
Element gaugeRight(float ratio);
Element gaugeUp(float ratio);
Element gaugeDown(float ratio);
Element gaugeDirection(float ratio, GaugeDirection);
```

## graph

@htmlonly
<script id="asciicast-223726" src="https://asciinema.org/a/223726.js" async></script>
@endhtmlonly

See:
```cpp
Element graph(GraphFunction);
```

## Colors

Most terminal consoles can display colored text and colored backgrounds. FTXUI supports every color palette:

~~~cpp
Decorator color(Color);
Decorator bgcolor(Color);
~~~
Color [gallery](https://arthursonzogni.github.io/FTXUI/examples_2dom_2color_gallery_8cpp-example.html):
![image](https://user-images.githubusercontent.com/4759106/147248595-04c7245a-5b85-4544-809d-a5984fc6f9e7.png)

### Palette16

On most terminals the following colors are supported:
- Default
- Black
- GrayDark
- GrayLight
- White
- Blue
- BlueLight
- Cyan
- CyanLight
- Green
- GreenLight
- Magenta
- MagentaLight
- Red
- RedLight
- Yellow
- YellowLight

Example use of the above colors using the pipe operator:
```cpp
text("Blue foreground") | color(Color::Blue);
text("Blue background") | bgcolor(Color::Blue);
text("Black on white") | color(Color::Black) | bgcolor(Color::White);
```

### Palette256

This palette is for terminals that support 256 colors.
@htmlonly
<script id="asciicast-OAUc3n6QrkmrLt7XEEb8AzbLt" src="https://asciinema.org/a/OAUc3n6QrkmrLt7XEEb8AzbLt.js" async></script>
@endhtmlonly

Example use of the 256 palette colors using the pipe operator:
```cpp
text("HotPink") | color(Color::HotPink);
```

### TrueColor

On terminal supporting trueColor, you can directly use the 24bit RGB color space:

Use the constructors below to specify the **RGB** or **HSV** values for your color:
```cpp
ftxui::Color::RGB(uint8_t red, uint8_t green, uint8_t blue);
ftxui::Color::HSV(uint8_t hue, uint8_t saturation, uint8_t value);
```

@htmlonly
<script id="asciicast-dk5r8IcCH0aFIIgWG0keSEHMG" src="https://asciinema.org/a/dk5r8IcCH0aFIIgWG0keSEHMG.js" async></script>
<script id="asciicast-xwzzghmqcqzIuyLwCpQFEqbEu" src="https://asciinema.org/a/xwzzghmqcqzIuyLwCpQFEqbEu.js" async></script>
@endhtmlonly

## Style

In addition to colored text and colored backgrounds. Many terminals support text effects such as: bold, dim, underlined, inverted, blink. 

~~~cpp
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element blink(Element);
Decorator color(Color);
Decorator bgcolor(Color);
~~~

To use these effects, simply wrap your elements with your desired effect:
~~~cpp
underlined(bold(text("This text is bold and underlined")))
~~~
Alternatively, use the pipe operator to chain it on your element:
~~~cpp
text("This text is bold")) | bold | underlined
~~~

Refer to [detailed style](https://arthursonzogni.github.io/FTXUI/examples_2dom_2style_gallery_8cpp-example.html) for a more detailed example.

![image](https://user-images.githubusercontent.com/4759106/147244118-380bf834-9e33-40df-9ff0-07c10f2598ef.png)

## Layout

Enables elements to be arranged in the following ways:
  - **Horizontally** with `ftxui::hbox`
  - **Vertically** with `ftxui::vbox`
  - **Inside a grid** with `ftxui::gridbox`
  - **Wrapped along one direction** using the `ftxui::flexbox`.
  
[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2vbox_hbox_8cpp-example.html) using `ftxui::hbox`, `ftxui::vbox` and `ftxui::filler`.

![image](https://user-images.githubusercontent.com/4759106/147242524-7103b5d9-1a92-4e2d-ac70-b3d6740061e3.png)
  
  
[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2gridbox_8cpp-example.htmlp) using `ftxui::gridbox`:

![image](https://user-images.githubusercontent.com/4759106/147242972-0db1f2e9-0790-496f-86e6-ed2c604f7a73.png)

[Example](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/hflow.cpp) using flexbox:

![image](https://user-images.githubusercontent.com/4759106/147243064-780ac7cc-605b-475f-94b8-cf7c4aed03a5.png)

Checkout this [example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2hflow_8cpp-example.html) and this Webassembly [demo](https://arthursonzogni.com/FTXUI/examples/?file=component/flexbox) for a more comprehensive look.

Element can also become flexible using the the `ftxui::flex` decorator.

**Example 1 (Pipe operator)**

Code:
~~~cpp
  hbox({
    text("left") | border ,
    text("middle") | border | flex,
    text("right") | border,
  });
~~~
Terminal Output:
~~~bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
~~~

**Example 2 (Pipe operator)**

Code:
~~~cpp
  hbox({
    text("left") | border ,
    text("middle") | border | flex,
    text("right") | border | flex,
  });
~~~
Terminal Output:
~~~bash
┌────┐┌───────────────────────────────────┐┌───────────────────────────────────┐
│left││middle                             ││right                              │
└────┘└───────────────────────────────────┘└───────────────────────────────────┘
~~~

## Table

Enables easy formatting of data into a neat table like visual form. 

[Code example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2table_8cpp-example.html)
  
![image](https://user-images.githubusercontent.com/4759106/147250766-77d8ec9e-cf2b-486d-9866-1fd9f1bd2e6b.png)

## Canvas

See [FTXUI Canvas struct documentation](https://arthursonzogni.github.io/FTXUI/structftxui_1_1Canvas.html)

Simple use case:
```cpp
  auto c = Canvas(100, 100);
  c.DrawPointLine(10, 10, 80, 10, Color::Red);
  auto element = canvas(c);
```

Drawing can be performed on a `ftxui::Canvas`, using braille, block, or simple
characters:
  
Simple [example](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/canvas.cpp):
  
![image](https://user-images.githubusercontent.com/4759106/147245843-76cc62fb-ccb4-421b-aacf-939f9afb42fe.png)

Complex [example](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/component/canvas_animated.cpp):
  
![ezgif com-gif-maker (3)](https://user-images.githubusercontent.com/4759106/147250538-783a8246-98e0-4a25-b032-3bd3710549d1.gif)  

# component

The `ftxui::component` module defines the logic that produces interactive components that respond to user events (keyboard, mouse, etc.).

A `ftxui::ScreenInteractive` defines a main loop that renders a component.

A `ftxui::Component` is a shared pointer to a `ftxui::ComponentBase`. The latter defines:
  - **`ftxui::ComponentBase::Render()`**: How to render the interface.
  - **`ftxui::ComponentBase::OnEvent()`**: How to react to events.
  - **`ftxui::ComponentBase::Add()`**: Construct a parent/child relationship between two components. This defines a tree for the components, which helps retrace ownership of the added components to help keyboard navigation.

`ftxui::Element` are used to render a single frame.

`ftxui::Component` are used to render dynamic user interfaces, capable of producing multiple frames and updating its state on events.

[Gallery code](https://arthursonzogni.github.io/FTXUI/examples_2component_2gallery_8cpp-example.html) example of multiple components. ([Gallery demo](https://arthursonzogni.com/FTXUI/examples/?file=component/gallery))

![image](https://user-images.githubusercontent.com/4759106/147247330-b60beb9f-e665-48b4-81c0-4b01ee95bc66.png)

All predefined components are available under [ftxui/dom](https://arthursonzogni.github.io/FTXUI/dir_8b052a5e35b6e4510790af7766fda6cf.htmll)

```cpp
#include <ftxui/component/component.hpp>
```

## Input

Captures user input characters, defined as `ftxui::Input()`. Refer to [FTXUI Input documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a30be54a28004c1c48c2a5ed54e4abd64) for more details.

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2input_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147247671-f1d6f606-1845-4e94-a4a0-d4273e9ae6bd.png)

@htmlonly
<script id="asciicast-223719" src="https://asciinema.org/a/223719.js" async></script>
@endhtmlonly

## Menu

Defines a menu object that contains a list of text, where the selected item is a focused element. Refer to [FTXUI Menu documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a777280bcec2414bef5c6725658c7714b) for more details on `ftxui::Menu()`.

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2menu_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147247822-0035fd6f-bb13-4b3a-b057-77eb9291582f.png)

@htmlonly
<script id="asciicast-223720" src="https://asciinema.org/a/223720.js" async></script>
@endhtmlonly

## Toggle

A horizontal list of elements, where only one unique item contained within it can be selected at one time, referenced by an index. The component is defined as `ftxui::Toggle()`. Component details are in [FTXUI Toggle documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a3537fccac66b07cd792874c9a6086811).

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2toggle_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147249383-e2201cf1-b7b8-4a5a-916f-d761e3e7ae40.png)

@htmlonly
<script id="asciicast-223722" src="https://asciinema.org/a/223722.js" async></script>
@endhtmlonly

## CheckBox

This component defines a checkbox. A list of toggle-label pairs that can be used to specify the selection of zero or many items simultaneously. It is produced by `ftxui::Checkbox()`. Further details can be found in [FTXUI Checkbox documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a04e40e348dee25bbbfce0a13bf234e19).

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2checkbox_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147246646-b86926a9-1ef9-4efb-af98-48a9b62acd81.png)

@htmlonly
<script id="asciicast-223724" src="https://asciinema.org/a/223724.js" async></script>
@endhtmlonly

## RadioBox

A radiobutton component. A list of toggle-label pairs that can be used to specify the selection of one unique item among the list. It is produced by `ftxui::RadioBox()`. Further details can be found in [FTXUI RadioBox documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a0d757f667caaff5287cb8702f52186ee).

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2radiobox_8cpp-example.html):
  
![image](https://user-images.githubusercontent.com/4759106/147246401-809d14a5-6621-4e36-8dd9-a2d75ef2a94e.png)

@htmlonly
<script id="asciicast-223725" src="https://asciinema.org/a/223725.js" async></script>
@endhtmlonly

## Dropdown

Functions similarly to the [RadioBox](#radiobox), but the items are displayed in a collapsible format. Can be created by `ftxui::Dropdown()` with more details highlighted in [FTXUI Dropdown documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#aa22bddff4aeed66e20f30d0715596999).

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2dropdown_8cpp-example.html):

![youtube-video-gif (3)](https://user-images.githubusercontent.com/4759106/147246982-1e821751-531c-4e1f-bc37-2fa290e143cd.gif)

## Slider

Represents a slider object that consists of a range with binned intermediate intervals. It can be created by `ftxui::Slider()`. Information regarding integer and floating type variants of this component can be found in [FTXUI Slider documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#acc572957ec06e5eb955856ee058f8d90).

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2slider_8cpp-example.html):

![image](https://user-images.githubusercontent.com/4759106/147249265-7e2cad75-082c-436e-affe-44a550c480ab.png)

## Renderer

Created by: `ftxui::Renderer()` from  `ftxui/component/component.hpp`. This
component decorates others by using a different function to render an interface.

Example:
```cpp
auto inner = [...]

auto renderer = Renderer(inner, [&] {
  return inner->Render() | border
});
```

`ftxui::Renderer` also supports the component decorator pattern:
```cpp
auto component = [...]
component = component
  | Renderer([] (Element e) { return e | border))
  | Renderer(bold)
```

In short form, you can also compose a component with element decorators:
```cpp
auto component = [...]
component = component | border | bold;
```

## CatchEvent

Produced by: `ftxui::CatchEvent()` from `ftxui/component/component.hpp`.
This component decorate others, catching events before the underlying component.

Examples:
```cpp
auto screen = ScreenInteractive::TerminalOutput();
auto renderer = Renderer([] {
  return text("My interface");
});
auto component = CatchEvent(renderer, [&](Event event) {
  if (event == Event::Character('q')) {
    screen.ExitLoopClosure()();
    return true;
  }
  return false;
});
screen.Loop(component);
```

The `ftxui::CatchEvent` can also be used as a decorator:
```cpp
component = component
  | CatchEvent(handler_1)
  | CatchEvent(handler_2)
  | CatchEvent(handler_3)
  ;
```

## Collapsible

Useful for visual elements whose visibility can be toggle on/off by the user.
Essentially, this the combination of the `ftxui::Checkbox()` and `ftxui::Maybe()` components.

```cpp
auto collabsible = Collapsible("Show more", inner_element);
```

## Maybe

Produced by: `ftxui::Maybe()` from `ftxui/component/component.hpp`.
This component can be utilized to show/hide any other component via a boolean or a predicate.

Example with a boolean:
```cpp
bool show = true;
auto component = Renderer([]{ return "Hello World!"; });
auto maybe_component = Maybe(component, &show)
```

Example with a predicate:
```cpp
auto component = Renderer([]{ return "Hello World!"; });
auto maybe_component = Maybe(component, [&] { return time > 10; })
```

As usual, `ftxui::Maybe` also can be used as a decorator:

```cpp
component = component
  | Maybe(&a_boolean)
  | Maybe([&] { return time > 10; })
  ;
```

## Container

### Horizontal

`ftxui::Container::Horizontal()` displays a list of components horizontally and handle keyboard/mouse navigation correspondingly. More information at [FTXUI Horizontal documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui_1_1Container.html#a01563bae9454fe64c0b29b0eb3755e68).

### Vertical

`ftxui::Container::Vertical()` displays a list of components vertically and handle keyboard/mouse navigation correspondingly. More information at [FTXUI Vertical documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui_1_1Container.html#a6471e39f5664d059144746c5801d37e6).

### Tab

`ftxui::Container::Tab()` is useful for implementing a tab bar, as it only allows one component from the underlying list to be displayed at once. More information at [FTXUI Tab documentation](https://arthursonzogni.github.io/FTXUI/namespaceftxui_1_1Container.html#aea9fe244eeeb420276bcc5760b46953d).

[Vertical Tab Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2tab_vertical_8cpp-example.html):

![ezgif com-gif-maker (1)](https://user-images.githubusercontent.com/4759106/147250144-22ff044a-4773-4ff7-a49c-12ba4034acb4.gif)

[Horizontal Tab Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2tab_horizontal_8cpp-example.html):
  ![ezgif com-gif-maker (2)](https://user-images.githubusercontent.com/4759106/147250217-fe447e0f-7a99-4e08-948a-995087d9b40e.gif)

## ResizableSplit

It defines a horizontal or vertical separation between two children
components. The position of the split is variable and controllable using the
mouse. 

There are four possible splits:
- [ftxui::ResizableSplitLeft()](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#ae2205fc172615b43e1e3307bb614be4c)
- [ftxui::ResizableSplitRight()](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a9030d0beb64f653603cab382b02750a8)
- [ftxui::ResizableSplitTop()](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#a238b4b650aca14716b82b2198695e008)
- [ftxui::ResizableSplitBottom()](https://arthursonzogni.github.io/FTXUI/namespaceftxui.html#acb75e6e61a021829893f7b57aeb7e3a2)

[Example](https://arthursonzogni.github.io/FTXUI/examples_2component_2resizable_split_8cpp-example.html):

![ezgif com-gif-maker](https://user-images.githubusercontent.com/4759106/147248372-c55512fe-9b96-4b08-a1df-d05cf2cae431.gif)  

@htmlonly
<script id="asciicast-tprMH2EdkUoMb7D2YxgMGgpzx" src="https://asciinema.org/a/tprMH2EdkUoMb7D2YxgMGgpzx.js" async></script>
@endhtmlonly

## Force a frame redraw

Typically, `ftxui::ScreenInteractive::Loop()` is responsible for drawing a new frame whenever a new group of events (e.g keyboard, mouse, window resize, etc.) has been processed. However, you might want to react to arbitrary events that are unknown to FTXUI. To accomplish this, you must post events using `ftxui::ScreenInteractive::PostEvent` (**this is thread safe**) via a thread. You will have to post the event `ftxui::Event::Custom`.

```cpp
screen->PostEvent(Event::Custom);
```
