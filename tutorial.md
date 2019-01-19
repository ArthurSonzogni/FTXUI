- [Introduction](#introduction)
- [DOM](#dom)
  * [Style](#style)
  * [Layout](#layout)
    + [Examples](#examples)
  * [Widget.](#widget)
    + [text](#text)
    + [border](#border)
    + [separator](#separator)
    + [gauge](#gauge)
  * [Decorator](#decorator)
- [Components.](#components)
  * [Input](#input)
  * [Menu](#menu)
  * [Toggle.](#toggle)

## Introduction
I highly recommand to not take too long on the tutorial, instead you should try
to understand the /example/*.

## DOM
All the dom element are declared in one header:
~~~cpp
#include <ftxui/dom/elements.hpp>
~~~

It declares the following set of elements:

~~~cpp
// --- Widget ---
Element text(std::wstring text);
Element separator();
Element gauge(float ratio);
Element border(Element);
Element window(Element title, Element content);
Element spinner(int charset_index, size_t image_index);

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
Element vbox(Elements);
Element hbox(Elements);
Element dbox(Elements);

// -- Flexibility ---
// Define how to share the remaining space when not all of it is used inside a
// container.
Element filler();
Element flex(Element);
Decorator size(size_t width, size_t height);

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
Element color(Color, Element);
Element bgcolor(Color, Element);
~~~

### Layout

* vbox (Vertical-box)
* hbox (Horizontal-box)
* dbox (Depth-box)
are containers. They are used to compose all the elements together. Each
children are put side by side. If the container is flexible, the extra space
available will be shared among the remaining flexible children.

flex(element) can be used to make a non-flexible element flexible. filler() is a
flexible empty element. You can use it align children on one side of the
container.

#### Examples
~~~cpp
  hbox(
    text(L"left") | border ,
    text(L"middle") | border | flex,
    text(L"right") | border
  );
~~~
~~~bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
~~~

~~~cpp
  hbox(
    text(L"left") | border ,
    text(L"middle") | border | flex,
    text(L"right") | border | flex
  );
~~~
~~~bash
┌────┐┌───────────────────────────────────┐┌───────────────────────────────────┐
│left││middle                             ││right                              │
└────┘└───────────────────────────────────┘└───────────────────────────────────┘
~~~

### Widget.

#### text

The most simple widget. It display a text.
~~~cpp
  text(L"I am a piece of text");
~~~
~~~bash
I am a piece of text.
~~~

#### border
Add a border arround an element
~~~cpp
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
border(hbox(
  vbox(
    text(L"left top"),
    text(L"left bottom")
  ),
  separator(),
  vbox(
    text(L"right top"),
    text(L"right bottom")
  )
));
~~~

~~~bash
┌───────────┬────────────┐
│left top   │right top   │
│left bottom│right bottom│
└───────────┴────────────┘
~~~

#### gauge

A gauge. It can be used to represent a progress bar.
~~~c+
border(gauge(0.5))
~~~

~~~bash
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

### Decorator

Terminal supports displaying text using differet style: bold, dim, underlined,
inverted, blink. It even support foreground and background color.

Example:
~~~cpp
underlined(bold(text(L"This text is bold and underlined")))
~~~

Tips: The pipe operator can be used to chain Decorator:
~~~cpp
text(L"This text is bold")) | bold | underlined
~~~

## Components.

dom element are stateless. 
### Input
  TODO(arthursonzogni): Add Video
### Menu
  TODO(arthursonzogni): Add Video
### Toggle.
  TODO(arthursonzogni): Add video
