Table of content.
- [DOM](#dom)
  * [Style](#style)
  * [Layout](#layout)
  * [Widget.](#widget)
    + [text](#text)
    + [frame](#frame)
    + [separator](#separator)
    + [gauge](#gauge)
- [Components.](#components)
  * [Input](#input)
  * [Menu](#menu)
  * [Toggle.](#toggle)

## DOM
All the dom element are declared in one header:
~~~cpp
#include <ftxui/dom/elements.hpp>
~~~

It declares the following set of elements:

~~~cpp
// --- Layout ----
Element vbox(Children);
Element hbox(Children);
Element dbox(Children);
Element filler();
Element flex(Element);

// --- Widget --
Element text(std::wstring text);
Element separator();
Element gauge(float ratio);
Element frame(Element);
Element window(Child title, Child content);

// -- Decorator ---
Element bold(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element blink(Element);
Element color(Color, Element);
Element bgcolor(Color, Element);
Decorator color(Color);
Decorator bgcolor(Color);

// --- Util ---
Element hcenter(Element);
Element vcenter(Element);
Element center(Element);

// --- Util ---
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
* fbox (Depth-box)
are containers. They are used to compose all the elements together. Each
children are put side by side. If the container is flexible, the extra space
available will be shared among the remaining flexible children.

flex(element) can be used to make a non-flexible element flexible.
filler() is a flexible empty element. You can use it to children on one side of
the container.

#### Examples
~~~cpp
  hbox(
    frame(text(L"left")),
    flex(frame(text(L"middle"))),
    frame(text(L"right"))
  );
~~~
~~~bash
┌────┐┌─────────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                           ││right│
└────┘└─────────────────────────────────────────────────────────────────┘└─────┘
~~~

~~~cpp
  hbox(
    frame(text(L"left")),
    flex(frame(text(L"middle"))),
    flex(frame(text(L"right")))
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

#### frame
Add a border arround an element
~~~cpp
frame(text(L"The element"))
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
frame(hbox(
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
frame(gauge(0.5))
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

### Input
  TODO(arthursonzogni): Add Video
### Menu
  TODO(arthursonzogni): Add Video
### Toggle.
  TODO(arthursonzogni): Add video
