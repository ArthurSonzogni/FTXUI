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

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
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
```cpp
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

# Modules

The project is split into 3 modules:
1. ftxui/screen
2. ftxui/dom
3. ftxui/component

ftxui/screen defines a ftxui::Screen, this is a grid of ftxui::Pixel.

ftxui/dom is the main module. It defines a hierarchical set of ftxui::Element.
An element draws something on the ftxui::Screen. It is responsive to the size of
its container.

ftxui/component. The part is only needed if you need to respond to the User
input. It defines a set of ftxui::Component.  The use can navigates using the
arrow keys and interact with widgets like checkbox/inputbox/... You can make you
own components.

## screen

It defines a ftxui::Screen. This is a grid of ftxui::Pixel. A Pixel represent a
unicode character and its style. The screen can be printed as a string using ftxui::Screen::ToString().

Example:
~~~cpp
  #include <ftxui/screen/screen.hpp>

  int main(void) {
    using namespace ftxui;
    auto screen = ftxui::Screen(Dimension::Fixed(32), Dimension::Fixed(10));

    auto& pixel = screen.PixelAt(10,10);
    pixel.character = U'A';
    pixel.bold = true;

    std::cout << screen.ToString();
    return EXIT_SUCCESS;
  }
~~~

## dom

This module defines a hierachical set of Element. An element manages layout and can be responsive to the terminal dimensions.

They are declared in ftxui/dom/elements.hpp>

\include ftxui/dom/elements.hpp

### text

The most simple widget. It displays a text.
~~~cpp
using namespace ftxui;

text(L"I am a piece of text");
~~~
~~~bash
I am a piece of text.
~~~

### border

Add a border around an element
~~~cpp
using namespace ftxui;

border(text(L"The element"))
~~~

~~~bash
┌───────────┐
│The element│
└───────────┘
~~~

### separator

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

### gauge

A gauge. It can be used to represent a progress bar.
~~~cpp
border(gauge(0.5))
~~~

~~~bash
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

### graph
[![asciicast](https://asciinema.org/a/223726.svg)](https://asciinema.org/a/223726)


### Style
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

### Layout

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

## Components.

Element are stateless object. On the other side, components are used when an
internal state is needed. Components are used to interact with the user with
its keyboard. They handle keyboard navigation, including component focus.

### Input
[![asciicast](https://asciinema.org/a/223719.svg)](https://asciinema.org/a/223719)

### Menu
[![asciicast](https://asciinema.org/a/223720.svg)](https://asciinema.org/a/223720)

### Toggle.
[![asciicast](https://asciinema.org/a/223722.svg)](https://asciinema.org/a/223722)

### CheckBox
[![asciicast](https://asciinema.org/a/223724.svg)](https://asciinema.org/a/223724)

### RadioBox
[![asciicast](https://asciinema.org/a/223725.svg)](https://asciinema.org/a/223725)
