@page module-screen ftxui / screen
@tableofcontents

![title-img](https://nsm09.casimages.com/img/2025/05/31//2505310207423242518595348.png)

The `ftxui::screen` module is the low-level foundation. It can be used
standalone, but it is primarily designed to be used together by
[ftxui::dom](module-dom.html) and [ftxui::component](module-component.html)
modules.

It provides a @ref ftxui::Screen.

---

# ftxui::Screen

The @ref ftxui::Screen class represents a 2D grid of styled characters that can
be rendered to a terminal.  
It provides methods to create a screen, access pixels, and render elements.

You can access the individual cells (@ref ftxui::Pixel) of the screen using 
the @ref ftxui::Screen::PixelAt method, which returns a reference
to the pixel at the specified coordinates.

**Example**
```cpp
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>

void main() {
    auto screen = ftxui::Screen::Create(
        ftxui::Dimension::Full(),   // Use full terminal width
        ftxui::Dimension::Fixed(10) // Fixed height of 10 rows
    );

    // Access a specific pixel at (10, 5)
    auto& pixel = screen.PixelAt(10, 5);

    // Set properties of the pixel.
    pixel.character = U'X';
    pixel.foreground_color = ftxui::Color::Red;
    pixel.background_color = ftxui::Color::RGB(0, 255, 0);
    pixel.bold = true; // Set bold style
    screen.Print(); // Print the screen to the terminal
}
```

> [!note]
> If the coordinates are out of bounds, a dummy pixel is returned.

The screen can be printed to the terminal using @ref ftxui::Screen::Print() or
converted to a std::string with @ref ftxui::Screen::ToString().

<div class="tabbed">
 
- <b class="tab-title">Print()</b>
  ```cpp
  auto screen = ...;
  screen.Print();
  ```
- <b class="tab-title">ToString()</b>
  ```cpp
  auto screen = ...;
  std::cout << screen.ToString();
  ```
 
</div>

Note that you can reset the cursor position to the top-left corner of the
screen after printing by calling @ref ftxui::Screen::ResetCursorPosition().

**Example**
```cpp
auto screen = ...;
while(true) {
  // Drawing operations:
  ...
  
  // Print the screen to the terminal. Then reset the cursor position and the
  // screen content.
  std::cout << screen.ToString();
  std::cout << screen.ResetCursorPosition(/*clear=*/true);
  std::cout << std::flush;

  // Sleep for a short duration to control the refresh rate.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

---

# ftxui::Dimension

The @ref ftxui::Dimension utility controls screen sizing:

* `Dimension::Full()` — use full terminal width or height
* `Dimension::Fit(element)` — size to fit the rendered @ref ftxui::Element
* `Dimension::Fixed(n)` — use exactly `n` columns or rows

These values are to be passed to `ftxui::Screen::Create()`.

@ref ftxui::Screen::Create() provides two overloads:

- `Screen::Create(Dimension)` sets both width and height to the same kind of dimension
- `Screen::Create(Dimension width, Dimension height)` allows distinct control per axis

```cpp
auto screen = ftxui::Screen::Create(
  ftxui::Dimension::Full(),      // width
  ftxui::Dimension::Fixed(10)    // height
);
```

Once created, render an element and display the result:

```cpp
ftxui::Render(screen, element);
screen.Print();
```

---

# ftxui::Pixel

Each cell in the screen grid is a @ref ftxui::Pixel, which holds:

- Unicode codepoint.
    - `character`
- @ref ftxui::Color:
    - `foreground_color`
    - `background_color`
- Booleans:
    - `blink`
    - `bold`
    - `dim`
    - `italic`
    - `inverted` (swap foreground and background colors)
    - `underlined`
    - `underlined_double`
    - `strikethrough`


```cpp
auto screen = ftxui::Screen::Create(
  ftxui::Dimension::Fixed(5),
  ftxui::Dimension::Fixed(5),
);

auto& pixel = screen.PixelAt(3, 3);
pixel.character = U'X';
pixel.bold = true;
pixel.foreground_color = ftxui::Color::Red;
pixel.background_color = ftxui::Color::RGB(0, 255, 0);

screen.Print();
```

> [!note]
> `PixelAt(x, y)` performs bounds checking and returns a reference to the pixel
> at the specified coordinate. If out-of-bounds, a dummy pixel reference is
> returned.


Each cell in the screen is a @ref ftxui::Pixel. You can modify them using:

```cpp
auto& pixel = screen.PixelAt(x, y);
pixel.character = U'X';
pixel.bold = true;
pixel.foreground_color = Color::Red;
```

---

# ftxui::Color

The @ref ftxui::Color class is used to define foreground and background colors for each @ref ftxui::Pixel.

It supports various color spaces and predefined palettes. FTXUI will
dynamically fallback to the closest available color in the terminal if the
requested color is not supported by the terminal.

**Color Spaces**

- **Default**: `ftxui::Color::Default` (terminal's default color)
- **16-color palette** [Demo](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/color_gallery):
    - `ftxui::Color::Black`,
    - `ftxui::Color::Red`,
    - ...
- **256-color palette** [Demo](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/color_palette256):
    - `ftxui::Color::Chartreuse1`,
    - `ftxui::Color::DarkViolet`,
    - ...
- **True color** (24bit) [Demo](https://arthursonzogni.github.io/FTXUI/examples/?file=dom/color_truecolor_HSV:
    - `ftxui::Color::RGB(uint8_t red, uint8_t green, uint8_t blue)`
    - `ftxui::Color::HSV(uint8_t h, uint8_t s, uint8_t v)`.
    

> [!note]
> You can query the terminal capability using @ref ftxui::Terminal::ColorSupport();
>
> This can manually be set using @ref ftxui::Terminal::SetColorSupport().
