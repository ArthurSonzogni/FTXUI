\mainpage

# Introduction

Welcome to the FTXUI documentation. Here, you will find the detail of every
functions and classes.

@tableofcontents 

**Short example**

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

CMakeLists.txt
~~~cmake
cmake_minimum_required (VERSION 3.11)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  # Specify a GIT_TAG here.
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

Build
~~~
mkdir build && cd build
cmake ..
make
./main
~~~

# List of modules.

The project is split into 3 modules:

1. **ftxui/screen** defines a `ftxui::Screen`, this is a grid of `ftxui::Pixel`.

2. **ftxui/dom** is the main module. It defines a hierarchical set of
   `ftxui::Element`.  An element draws something on the `ftxui::Screen`. It is
   responsive to the size of its container.

3. **ftxui/component** The part is only needed if you need to respond to the
   User input. It defines a set of `ftxui::Component`.  The use can navigates
   using the arrow keys and interact with widgets like checkbox/inputbox/... You
   can make you own components.

# screen

It defines a `ftxui::Screen`. This is a grid of `ftxui::Pixel`. A Pixel
represent a Unicode character and its associated style (bold, colors, etc...).
The screen can be printed as a string using `ftxui::Screen::ToString()`.

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

This module defines a hierarchical set of Element. An element manages layout and
can be responsive to the terminal dimensions.

**Example:**
```cpp
// Define the document
Element document = vbox({
    text("The window") | bold | color(Color::Blue),
    gauge(0.5)
    text("The footer")
  });

// Add a border.
document = border(document);
```

**List of elements**

You only need one header: ftxui/dom/elements.hpp

\include ftxui/dom/elements.hpp

## text

The most simple widget. It displays a text.
~~~cpp
text("I am a piece of text");
~~~
~~~bash
I am a piece of text.
~~~

## border

Add a border around an element
~~~cpp
border(text("The element"))
~~~

~~~bash
┌───────────┐
│The element│
└───────────┘
~~~

## window

A `ftxui::window` is a `ftxui::border`, but with some text on top of the border.
Add a border around an element
~~~cpp
window("The window", text("The element"))
~~~

~~~bash
┌The window─┐
│The element│
└───────────┘
~~~

## separator

Display a vertical or horizontal line to visually split the content of a
container in two.

~~~cpp
border(
  hbox({
    text("Left"), 
    separator(),
    text("Right")
  })
)
~~~

~~~bash
┌────┬─────┐
│left│right│
└────┴─────┘
~~~

## gauge

A gauge. It can be used to represent a progress bar.
~~~cpp
border(gauge(0.5))
~~~

~~~bash
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

## graph

@htmlonly
<script id="asciicast-223726" src="https://asciinema.org/a/223726.js" async></script>
@endhtmlonly

## Colors
A terminal console can usually display colored text and colored background.

~~~cpp
Decorator color(Color);
Decorator bgcolor(Color);
~~~

### Palette16

On most terminal the following colors are supported:
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

Example:
```cpp
text("Blue foreground") | color(Color::Blue);
text("Blue background") | bgcolor(Color::Blue);
text("Black on white") | color(Color::Black) | bgcolor(Color::White);
```

### Palette256

On terminal supporting 256 colors. 
@htmlonly
<script id="asciicast-OAUc3n6QrkmrLt7XEEb8AzbLt" src="https://asciinema.org/a/OAUc3n6QrkmrLt7XEEb8AzbLt.js" async></script>
@endhtmlonly

```cpp
text("HotPink") | color(Color::HotPink);
```

### TrueColor

On terminal supporting trueColor, you can directly chose the 24bit RGB color:

There are two constructors:
```cpp
ftxui::Color::RGB(uint8_t red, uint8_t green, uint8_t blue);
ftxui::Color::HSV(uint8_t hue, uint8_t saturation, uint8_t value);
```

@htmlonly
<script id="asciicast-dk5r8IcCH0aFIIgWG0keSEHMG" src="https://asciinema.org/a/dk5r8IcCH0aFIIgWG0keSEHMG.js" async></script>
<script id="asciicast-xwzzghmqcqzIuyLwCpQFEqbEu" src="https://asciinema.org/a/xwzzghmqcqzIuyLwCpQFEqbEu.js" async></script>
@endhtmlonly

## Style
A terminal console can usually display colored text and colored background.
The text can also have different effects: bold, dim, underlined, inverted,
blink.

~~~cpp
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element blink(Element);
Decorator color(Color);
Decorator bgcolor(Color);
~~~

Example:
~~~cpp
underlined(bold(text("This text is bold and underlined")))
~~~

Tips: The pipe operator can be used to chain Decorator:
~~~cpp
text("This text is bold")) | bold | underlined
~~~

## Layout

These layout are similar to the HTML flexbox:
* vbox (Vertical-box)
* hbox (Horizontal-box)
* dbox (Z-axis-box)
They are used to compose all the elements together. Each
children are put side by side. If the container is flexible, the extra space
available will be shared among the remaining flexible children.

`flex(element)` can be used to make a non-flexible element flexible. `filler()`
is a flexible empty element. You can use it align children on one side of the
container.

An horizontal flow layout is implemented by:
* hflow (Horizontal flow)

**Examples**
~~~cpp
  hbox({
    text("left") | border ,
    text("middle") | border | flex,
    text("right") | border,
  });
~~~
~~~bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
~~~

~~~cpp
  hbox({
    text("left") | border ,
    text("middle") | border | flex,
    text("right") | border | flex,
  });
~~~
~~~bash
┌────┐┌───────────────────────────────────┐┌───────────────────────────────────┐
│left││middle                             ││right                              │
└────┘└───────────────────────────────────┘└───────────────────────────────────┘
~~~


# component

The `ftxui/component` directory defines the logic to get produce
interactive component responding to user's events (keyboard, mouse, etc...)

A `ftxui::ScreenInteractive` defines a main loop to render a component.

A `ftxui::Component` is a shared pointer to a `ftxui::ComponentBase`. The later
defines
  - `ftxui::ComponentBase::Render()`: How to render the interface.
  - `ftxui::ComponentBase::OnEvent()`: How to react to events.
  - `ftxui::ComponentBase::Add()`: Give a parent/child relation ship in between
    two component. This defines a tree a components, which help properly define
    how keyboard navigation works.

Predefined components are available in `ftxui/dom/component.hpp`:

\include ftxui/component/component.hpp

Element are stateless object. On the other side, components are used when an
internal state is needed. Components are used to interact with the user with
its keyboard. They handle keyboard navigation, including component focus.

## Input

Produced by: `ftxui::Input()` from "ftxui/component/component.hpp"

@htmlonly
<script id="asciicast-223719" src="https://asciinema.org/a/223719.js" async></script>
@endhtmlonly

## Menu

Produced by: `ftxui::Menu()` from "ftxui/component/component.hpp"

@htmlonly
<script id="asciicast-223720" src="https://asciinema.org/a/223720.js" async></script>
@endhtmlonly

## Toggle.

Produced by: `ftxui::Toggle()` from "ftxui/component/component.hpp"

@htmlonly
<script id="asciicast-223722" src="https://asciinema.org/a/223722.js" async></script>
@endhtmlonly

## CheckBox

Produced by: `ftxui::Checkbox()` from "ftxui/component/component.hpp"

@htmlonly
<script id="asciicast-223724" src="https://asciinema.org/a/223724.js" async></script>
@endhtmlonly

## RadioBox

Produced by: `ftxui::Radiobox()` from "ftxui/component/component.hpp"

@htmlonly
<script id="asciicast-223725" src="https://asciinema.org/a/223725.js" async></script>
@endhtmlonly

## Renderer

Produced by: `ftxui::Renderer()` from \ref "ftxui/component/component.hpp". This
component decorate another one by using a different function to render an
interface.

## CatchEvent 

Produced by: `ftxui::CatchEvent()` from \ref "ftxui/component/component.hpp".
This component decorate another one and catch the events before the underlying
component.

## Container::Horizontal 

Produced by: `ftxui::Container::Horizontal()` from
"ftxui/component/component.hpp". It displays a list of components horizontally
and handle keyboard/mouse navigation.

## Container::Vertial

Produced by: `ftxui::Container::Vertical()` from
"ftxui/component/component.hpp". It displays a list of components vertically 
and handles keyboard/mouse navigation.

## Container::Tab

Produced by: `ftxui::Container::Tab()` from
"ftxui/component/component.hpp". It take a list of component and display only
one of them. This is useful for implementing a tab bar.

## ResizableSplit::{Left, Right, Top, Bottom} 

Produced by:
- `ftxui::ResizableSplitLeft()`
- `ftxui::ResizableSplitRight()`
- `ftxui::ResizableSplitTop()`
- `ftxui::ResizableSplitBottom()`
from "ftxui/component/component.hpp"

It defines an horizontal or vertical separation in between two children
component. The position of the split is variable and controllable using the
mouse.

@htmlonly
<script id="asciicast-tprMH2EdkUoMb7D2YxgMGgpzx" src="https://asciinema.org/a/tprMH2EdkUoMb7D2YxgMGgpzx.js" async></script>
@endhtmlonly

## Force a frame redraw.

Whenever a new group of events have been processed: keyboard, mouse, window
resize, etc..., the `ftxui::ScreenInteractive::Loop()` is responsible for
drawing a new frame.

You might want to react to arbitrary events that are unknown to FTXUI. This can
be achieve by posting events via `ftxui::ScreenInteractive::PostEvent`, via a
thread. You can post the event`ftxui::Event::Custom`.

```cpp
screen->PostEvent(Event::Custom);
```

`ftxui::ScreenInteractive::PostEvent` is thread safe.
