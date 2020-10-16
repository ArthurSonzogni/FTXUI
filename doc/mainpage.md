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
      text(L"left")   | border,
      text(L"middle") | border | flex,
      text(L"right")  | border,
    });

  auto screen = Screen::Create(
    Dimension::Full(),       // Width
    Dimension::Fit(document) // Height
  );
  Render(screen, document);
  std::cout << screen.ToString() << std::endl;

  return EXIT_SUCCESS;
}
```

**output**
```bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
```

**cmake**
```c
cmake_minimum_required (VERSION 3.11)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
)
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(main src/main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)
set_target_properties(main PROPERTIES CXX_STANDARD 17)
```

# List of modules.

The project is split into 3 modules:

1. **ftxui/screen** defines a ftxui::Screen, this is a grid of ftxui::Pixel.

2. **ftxui/dom** is the main module. It defines a hierarchical set of ftxui::Element.
An element draws something on the ftxui::Screen. It is responsive to the size of
its container.

3. **ftxui/component** The part is only needed if you need to respond to the User
input. It defines a set of ftxui::Component.  The use can navigates using the
arrow keys and interact with widgets like checkbox/inputbox/... You can make you
own components.

## screen

It defines a ftxui::Screen. This is a grid of ftxui::Pixel. A Pixel represent a
unicode character and its associated style (bold, colors, etc...).
The screen can be printed as a string using ftxui::Screen::ToString().

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

## dom

This module defines a hierachical set of Element. An element manages layout and can be responsive to the terminal dimensions.


**Example:**
```cpp
// Define the document
Element document = vbox({
    text(L"The window") | bold | color(Color::Blue),
    gauge(0.5)
    text(L"The footer")
  });

// Add a border.
document = border(document);
```

**List of elements**

You only need one header: ftxui/dom/elements.hpp

\include ftxui/dom/elements.hpp

## component

Finally, the ftxui/component directory defines the logic to get interactivity.

Please take a look at ./examples/component

This provides:
1. A main loop.
2. Get events and respond to them.
3. A predefined implementation of "keyboard navigation".
4. A set of predefined widget: CheckBox, RadioBox, Input, Menu, Toggle.

# ftxui/dom

Every elements of the dom are declared from:
\ref ftxui/dom/elements.hpp

## text

The most simple widget. It displays a text.
~~~cpp
text(L"I am a piece of text");
~~~
~~~bash
I am a piece of text.
~~~

## border

Add a border around an element
~~~cpp
border(text(L"The element"))
~~~

~~~bash
┌───────────┐
│The element│
└───────────┘
~~~

## window

A ftxui::window is a ftxui::border, but with some text on top of the border.
Add a border around an element
~~~cpp
window(L"The window", text(L"The element"))
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
    text(L"Left"), 
    separator(),
    text(L"Right")
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
text(L"Blue foreground") | color(Color::Blue);
text(L"Blue backgrond") | bgcolor(Color::Blue);
text(L"Black on white") | color(Color::Black) | bgcolor(Color::White);
```

### Palette256

On terminal supporting 256 colors. 
@htmlonly
<script id="asciicast-OAUc3n6QrkmrLt7XEEb8AzbLt" src="https://asciinema.org/a/OAUc3n6QrkmrLt7XEEb8AzbLt.js" async></script>
@endhtmlonly

```cpp
text(L"HotPink") | color(Color::HotPink);
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
underlined(bold(text(L"This text is bold and underlined")))
~~~

Tips: The pipe operator can be used to chain Decorator:
~~~cpp
text(L"This text is bold")) | bold | underlined
~~~

## Layout

These layout are similar to the HTML flexbox:
* vbox (Vertical-box)
* hbox (Horizontal-box)
* dbox (Z-axis-box)
They are used to compose all the elements together. Each
children are put side by side. If the container is flexible, the extra space
available will be shared among the remaining flexible children.

flex(element) can be used to make a non-flexible element flexible. filler() is a
flexible empty element. You can use it align children on one side of the
container.

An horizontal flow layout is implemented by:
* hflow (Horizontal flow)

**Examples**
~~~cpp
  hbox({
    text(L"left") | border ,
    text(L"middle") | border | flex,
    text(L"right") | border,
    });
~~~
~~~bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
~~~

~~~cpp
  hbox({
    text(L"left") | border ,
    text(L"middle") | border | flex,
    text(L"right") | border | flex,
    });
~~~
~~~bash
┌────┐┌───────────────────────────────────┐┌───────────────────────────────────┐
│left││middle                             ││right                              │
└────┘└───────────────────────────────────┘└───────────────────────────────────┘
~~~

# ftxui/component

Element are stateless object. On the other side, components are used when an
internal state is needed. Components are used to interact with the user with
its keyboard. They handle keyboard navigation, including component focus.

## Input

The component: \ref ftxui::Input

@htmlonly
<script id="asciicast-223719" src="https://asciinema.org/a/223719.js" async></script>
@endhtmlonly

## Menu

The component: \ref ftxui::Menu

@htmlonly
<script id="asciicast-223720" src="https://asciinema.org/a/223720.js" async></script>
@endhtmlonly

## Toggle.

The component: \ref ftxui::Toggle

@htmlonly
<script id="asciicast-223722" src="https://asciinema.org/a/223722.js" async></script>
@endhtmlonly

## CheckBox

The component: \ref ftxui::CheckBox

@htmlonly
<script id="asciicast-223724" src="https://asciinema.org/a/223724.js" async></script>
@endhtmlonly

## RadioBox

The component: \ref ftxui::RadioBox

@htmlonly
<script id="asciicast-223725" src="https://asciinema.org/a/223725.js" async></script>
@endhtmlonly

# Build

Assuming this example example.cpp file.

**main.cpp**
~~~cpp
#include "ftxui/screen/screen.c
#include "ftxui/dom/elements.c
#include <iostream>

int main(int argc, const char *argv[]) {
  using namespace ftxui;
  auto document =
    hbox({
      text(L"left") | bold | border,
      text(L"middle") | flex | border,
      text(L"right") | border,
    });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}
~~~

## Using CMake

CMakeLists.txt
~~~cmake
cmake_minimum_required (VERSION 3.11)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  # Specify a GIT TAG here.
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

# C++17 is used. We requires fold expressions at least.
set_target_properties(ftxui-starter PROPERTIES CXX_STANDARD 17)

~~~

Build
~~~
mkdir build && cd build
cmake ..
make
./main
~~~

## Using NXXM

**.nxxm/deps**
~~~json
{
  "ArthurSonzogni/FTXUI": {}
}
~~~

Build:
~~~
nxxm . -t clang-cxx17
~~~
