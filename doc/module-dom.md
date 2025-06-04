@page module-dom ftxui / dom
@tableofcontents

![title-img](https://nsm09.casimages.com/img/2025/05/31//2505310207423242518595347.png)

This module defines a hierarchical set of `ftxui::Element`. An element manages
the layout and can be responsive to the terminal dimension changes. Note the
following example where this module is used to create a simple layout with a
number of operators:

The @subpage module-dom-examples section provides a collection of examples.

**Example:**
```cpp
namespace ftxui {
    ...

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

...
}
```

**List of elements**

The list of all elements are included and can be accessed by including the
corresponding header file:
```cpp
#include <ftxui/dom/elements.hpp>
```

\include{strip} "ftxui/dom/elements.hpp"

# text # {#dom-text}

The most simple widget. It displays a text.
```cpp
text("I am a piece of text");
```
```bash
I am a piece of text.
```

# vtext {#dom-vtext}

Identical to `ftxui::text`, but displayed vertically.

Code:
```cpp
vtext("HELLO");
```

Terminal output:
```bash
H
E
L
L
O
```

# paragraph {#dom-paragraph}

Similar to `ftxui::text`, but the individual word are wrapped along multiple
lines, depending on the width of its container.

Sample Code:
```cpp
paragraph("A very long text")
```

![ezgif com-gif-maker (4)](https://user-images.githubusercontent.com/4759106/147251370-983a06e7-6f41-4113-92b8-942f43d34d06.gif)

For a more detailed example refer to [detailed example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2paragraph_8cpp-example.html). Paragraph also includes a number of other variants as shown below:
```cpp
namespace ftxui {
    Element paragraph(std::string text);
    Element paragraphAlignLeft(std::string text);
    Element paragraphAlignRight(std::string text);
    Element paragraphAlignCenter(std::string text);
    Element paragraphAlignJustify(std::string text);
}
```


# border {#dom-border}

Adds a border around an element.

Code:
```cpp
border(text("The element"))
```

Terminal output:
```bash
┌───────────┐
│The element│
└───────────┘
```

> [!note]
> You can achieve the same behavior by using the pipe operator.
> 
> Code:
> ```cpp
> text("The element") | border
> ```

Border also comes in a variety of styles as shown below:
```cpp
namespace ftxui {
    Element border(Element);
    Element borderLight(Element);
    Element borderHeavy(Element);
    Element borderDouble(Element);
    Element borderRounded(Element);
    Element borderEmpty(Element);
    Decorator borderStyled(BorderStyle);
    Decorator borderWith(Pixel);
}
```


# window # {#dom-window}

A `ftxui::window` is a `ftxui::border`, but with an additional header. To add a
window around an element, wrap it and specify a string as the header.
Code:
```cpp
window("The window", text("The element"))
```

Terminal output:
```bash
┌The window─┐
│The element│
└───────────┘
```

# separator {#dom-separator}

Displays a vertical/horizontal line to visually split the content of a
container in two.

Code:
```cpp
border(
  hbox({
    text("Left"), 
    separator(),
    text("Right")
  })
)
```

Terminal output:
```bash
┌────┬─────┐
│left│right│
└────┴─────┘
```


Separators come in a variety of flavors as shown below:
```cpp
namespace ftxui {
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
}
```

# gauge {#dom-gauge}

This is a visual element that represents a ratio of progress.

Code:
```cpp
border(gauge(0.5))
```

Terminal output:
```bash
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
```

Gauges can be displayed in many orientations as shown below:
```cpp
namespace {
    Element gauge(float ratio);
    Element gaugeLeft(float ratio);
    Element gaugeRight(float ratio);
    Element gaugeUp(float ratio);
    Element gaugeDown(float ratio);
    Element gaugeDirection(float ratio, GaugeDirection);
}
```

# graph {#dom-graph}

@htmlonly
<script id="asciicast-223726" src="https://asciinema.org/a/223726.js" async></script>
@endhtmlonly

See:
```cpp
Element graph(GraphFunction);
```

# Colors {#dom-colors}

Most terminal consoles can display colored text and colored backgrounds. FTXUI
supports every color palette:
```cpp
Decorator color(Color);
Decorator bgcolor(Color);
```


Color [gallery](https://arthursonzogni.github.io/FTXUI/examples_2dom_2color_gallery_8cpp-example.html):
![image](https://user-images.githubusercontent.com/4759106/147248595-04c7245a-5b85-4544-809d-a5984fc6f9e7.png)

## Palette16 #{#dom-colors-palette-16}

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

## Palette256 #{#dom-colors-palette-256}

On terminal supporting 256 colors. 
@htmlonly
<script id="asciicast-OAUc3n6QrkmrLt7XEEb8AzbLt" src="https://asciinema.org/a/OAUc3n6QrkmrLt7XEEb8AzbLt.js" async></script>
@endhtmlonly

```cpp
text("HotPink") | color(Color::HotPink);
```

## TrueColor #{#dom-colors-true-color}

On terminal supporting trueColor, you can directly use the 24bit RGB color
space:

Use the constructors below to specify the **RGB** or **HSV** values for your
color:

There are two constructors:
```cpp
ftxui::Color::RGB(uint8_t red, uint8_t green, uint8_t blue);
ftxui::Color::HSV(uint8_t hue, uint8_t saturation, uint8_t value);
```

@htmlonly
<script id="asciicast-dk5r8IcCH0aFIIgWG0keSEHMG" src="https://asciinema.org/a/dk5r8IcCH0aFIIgWG0keSEHMG.js" async></script>
<script id="asciicast-xwzzghmqcqzIuyLwCpQFEqbEu" src="https://asciinema.org/a/xwzzghmqcqzIuyLwCpQFEqbEu.js" async></script>
@endhtmlonly

# LinearGradient #{#dom-linear-gradient}

FTXUI supports linear gradient. Either on the foreground or the background.

```cpp
Decorator color(const LinearGradient&);
Decorator bgcolor(const LinearGradient&);
```

A `ftxui::LinearGradient` is defined by an angle in degree, and a list of color
stops.
```cpp
auto gradient = LinearGradient()
  .Angle(45)
  .AddStop(0.0, Color::Red)
  .AddStop(0.5, Color::Green)
  .AddStop(1.0, Color::Blue);
```

You can also use simplified constructors:
```cpp
LinearGradient(Color::Red, Color::Blue);
```
```cpp
LinearGradient(45, Color::Red, Color::Blue);
```

See [demo](https://arthursonzogni.github.io/FTXUI/examples/?file=component/linear_gradient_gallery).


# Style {#dom-style}
In addition to colored text and colored backgrounds. Many terminals support text
effects such as: `bold`, `italic`, `dim`, `underlined`, `inverted`, `blink`.

```cpp
Element bold(Element);
Element italic(Element);
Element dim(Element);
Element inverted(Element);
Element underlined(Element);
Element underlinedDouble(Element);
Element strikethrough(Element);
Element blink(Element);
Decorator color(Color);
Decorator bgcolor(Color);
Decorator colorgrad(LinearGradient);
Decorator bgcolorgrad(LinearGradient);
```

[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2style_gallery_8cpp-example.html)

![image](https://user-images.githubusercontent.com/4759106/147244118-380bf834-9e33-40df-9ff0-07c10f2598ef.png)

To use these effects, simply wrap your elements with your desired effect:
```cpp
underlined(bold(text("This text is bold and underlined")))
```

Alternatively, use the pipe operator to chain it on your element:
```cpp
text("This text is bold") | bold | underlined
```

# Layout {#dom-layout}

Enables elements to be arranged in the following ways:
  - **Horizontally** with `ftxui::hbox`
  - **Vertically** with `ftxui::vbox`
  - **Inside a grid** with `ftxui::gridbox`
  - **Wrapped along one direction** using the `ftxui::flexbox`.
  
[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2vbox_hbox_8cpp-example.html) using `ftxui::hbox`, `ftxui::vbox` and `ftxui::filler`.

![image](https://user-images.githubusercontent.com/4759106/147242524-7103b5d9-1a92-4e2d-ac70-b3d6740061e3.png)
  
  
[Example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2gridbox_8cpp-example.htmlp)
using `ftxui::gridbox`:

![image](https://user-images.githubusercontent.com/4759106/147242972-0db1f2e9-0790-496f-86e6-ed2c604f7a73.png)

[Example](https://github.com/ArthurSonzogni/FTXUI/blob/master/examples/dom/hflow.cpp)
using flexbox:

![image](https://user-images.githubusercontent.com/4759106/147243064-780ac7cc-605b-475f-94b8-cf7c4aed03a5.png)

Checkout this
[example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2hflow_8cpp-example.html)
and the associated
[demo](https://arthursonzogni.github.io/FTXUI/examples/?file=component/flexbox).

Element can also become flexible using the `ftxui::flex` decorator.

Code:
```cpp
  hbox({
    text("left") | border ,
    text("middle") | border | flex,
    text("right") | border,
  });
```
Terminal output:
```bash
┌────┐┌─────────────────────────────────────────────────────┐┌─────┐
│left││middle                                               ││right│
└────┘└─────────────────────────────────────────────────────┘└─────┘
```

Code:
```cpp
  hbox({
    text("left") | border ,
    text("middle") | border | flex,
    text("right") | border | flex,
  });
```

Terminal output:
```bash
┌────┐┌───────────────────────────────┐┌───────────────────────────────┐
│left││middle                         ││right                          │
└────┘└───────────────────────────────┘└───────────────────────────────┘
```

# Table {#dom-table}

Enables easy formatting of data into a neat table like visual form.

[Code example](https://arthursonzogni.github.io/FTXUI/examples_2dom_2table_8cpp-example.html):
  
![image](https://user-images.githubusercontent.com/4759106/147250766-77d8ec9e-cf2b-486d-9866-1fd9f1bd2e6b.png)

# Canvas {#dom-canvas}

See the API [<ftxui/dom/canvas.hpp>](./canvas_8hpp_source.html)

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
