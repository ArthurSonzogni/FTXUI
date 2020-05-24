## Introduction

The library is split in 3 parts:

1. ftxui/screen/
2. ftxui/dom/
3. ftxui/component/

# ftxui/screen/

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


2. [ftuix/dom/](../include/ftxui/dom/)
  These are a set of hierachical ftxui::Element declared in
  [ftxui/dom/elements.hpp](../ftxui/dom/elements.hpp).  An element can render
  itself on the the Screen. It manage layout and is responsive to the Screen
  dimensions.

3. [ftuix/component/](../include/ftxui/component)
  The part is only needed if you need to respond to the User input. It defines a
  set of ftxui::Component.  The use can navigates using the arrow keys and
  interact with widgets like checkbox/inputbox/... You can make you own
  components.

~~~cpp
  using namespace ftxui;

  auto document = text(L"Hello world");

  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();
~~~


I recommand not to take too much time reading the tutorial. Instead,
you should try to read the examples (in [../examples/](../examples)).

## DOM
All the dom element are declared in one header:

\include ./ftxui/dom/elements.hpp

It declares the following set of elements:

~~~cpp
// --- Widget ---
Element text(std::wstring text);
Element separator();
Element separator(Pixel);
Element gauge(float ratio);
Element border(Element);
Decorator borderWith(Pixel);
Element window(Element title, Element content);
Element spinner(int charset_index, size_t image_index);
Elements paragraph(std::wstring text); // Use inside hflow(). Split by space.
Element graph(GraphFunction);

// -- Decorator ---
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element blink(Element);
Decorator color(Color);
Decorator bgcolor(Color);
Element color(Color, Element);
Element bgcolor(Color, Element);

// --- Layout ---
// Horizontal, Vertical or stacked set of elements.
Element hbox(Elements);
Element vbox(Elements);
Element dbox(Elements);
Element hflow(Elements);

// -- Flexibility ---
// Define how to share the remaining space when not all of it is used inside a
// container.
Element filler();
Element flex(Element);
Element notflex(Element);

// -- Size override;
enum Direction { WIDTH, HEIGHT };
enum Constraint { LESS_THAN, EQUAL, GREATER_THAN };
Decorator size(Direction, Constraint, int value);

// --- Frame ---
// A frame is a scrollable area. The internal area is potentially larger than
// the external one. The internal area is scrolled in order to make visible the
// focused element.
Element frame(Element);
Element focus(Element);
Element select(Element);

// --- Util --------------------------------------------------------------------
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);
Element align_right(Element);
Element nothing(Element element);
~~~

### Widget.

#### text

The most simple widget. It displays a text.
~~~cpp
using namespace ftxui;

text(L"I am a piece of text");
~~~
~~~bash
I am a piece of text.
~~~

#### border
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

#### separator

Display a vertical or horizontal line to visually split the content of a
container in two.

~~~cpp
border(
  hbox({
    vbox({
      text(L"left top"),
      text(L"left bottom"),
    }),
    separator(),
    vbox({
      text(L"right top"),
      text(L"right bottom"),
    }),
  })
);
~~~

~~~bash
┌───────────┬────────────┐
│left top   │right top   │
│left bottom│right bottom│
└───────────┴────────────┘
~~~

#### gauge

A gauge. It can be used to represent a progress bar.
~~~cpp
border(gauge(0.5))
~~~

~~~bash
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

#### graph
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

#### Examples
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
![asciicast](https://asciinema.org/a/223719.svg)

### Menu
[![asciicast](https://asciinema.org/a/223720.svg)](https://asciinema.org/a/223720)

### Toggle.
[![asciicast](https://asciinema.org/a/223722.svg)](https://asciinema.org/a/223722)

### CheckBox
[![asciicast](https://asciinema.org/a/223724.svg)](https://asciinema.org/a/223724)

### RadioBox
[![asciicast](https://asciinema.org/a/223725.svg)](https://asciinema.org/a/223725)
