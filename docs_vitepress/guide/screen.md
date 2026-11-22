# Screen Module (`ftxui/screen`)

The **Screen** module is the low-level foundation of FTXUI. It manages a 2D grid of styled terminal cells (`Screen`, `Surface`, `Cell`, `Color`, `Box`) and terminal capability detection.

## `Screen` & `Cell`

`Screen` represents a 2D grid of `Cell`s. You can modify cells directly with `screen.CellAt(x, y)` (which is bounds-checked and safely returns a dummy cell if out of bounds):

```cpp
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

int main() {
  using namespace ftxui;

  auto screen = Screen::Create(
      Dimension::Full(),    // Full terminal width
      Dimension::Fixed(10)  // Fixed height of 10 rows
  );

  auto& cell = screen.CellAt(10, 5);
  cell.character = "X";
  cell.foreground_color = Color::Red;
  cell.background_color = Color::RGB(0, 255, 0);
  cell.bold = true;

  screen.Print();
}
```

Each `Cell` stores:
- `character`: UTF-8 grapheme string
- `foreground_color`, `background_color`: `Color`
- Style flags: `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlined_double`, `blink`, `strikethrough`

## Printing & Redrawing

::: code-group

```cpp [Print directly]
screen.Print();
```

```cpp [Convert to string]
std::cout << screen.ToString();
```

:::

To redraw in place in a loop without scrolling the terminal, print `screen.ResetPosition()` before the next frame:

```cpp
std::string reset_position;
while (running) {
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  std::cout << reset_position;
  screen.Print();
  reset_position = screen.ResetPosition();
}
```

## `Dimension`

`Dimension` controls `Screen::Create(width, height)` sizing:

- `Dimension::Full()`: match the current terminal width or height
- `Dimension::Fit(element)`: fit the minimum size required by an `Element`
- `Dimension::Fixed(n)`: exact column or row count

## `Color` & Terminal Capabilities

`Color` supports 4 modes and automatically falls back to the closest color supported by the terminal:

- **Default:** `Color::Default`
- **16-color palette:** `Color::Red`, `Color::BlueLight`, … ([live demo](/examples/dom/color_gallery))
- **256-color palette:** `Color::Palette256`, `Color::Chartreuse1`, … ([live demo](/examples/dom/color_info_palette256))
- **24-bit TrueColor:** `Color::RGB(r, g, b)`, `Color::HSV(h, s, v)` ([live demo](/examples/dom/color_truecolor_HSV))

You can inspect or override color support with `Terminal::ColorSupport()` and `Terminal::SetColorSupport(...)` (see the [`ftxui/screen` API Reference](/api/screen)).
