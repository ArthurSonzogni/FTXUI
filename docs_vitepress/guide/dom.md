# DOM Module (`ftxui/dom`)

The **DOM** module defines a declarative, stateless tree of `Element` nodes that lay themselves out responsively to the terminal size.

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

using namespace ftxui;

auto title = text("The Window") | bold | color(Color::Blue);
auto progress = gauge(0.5f);
auto footer = text("The Footer") | dim;

auto document = vbox({
                    title,
                    progress,
                    footer,
                })
                | border;
```

Decorators can be applied in three equivalent ways:
- Function call: `document = border(document);`
- Pipe operator: `document = document | border;`
- In-place pipe: `document |= border;`

## Text & Paragraphs

- `text(str)`: single-line UTF-8 text (full-width, CJK, and emoji aware).
- `vtext(str)`: vertical text (one grapheme per row).
- `paragraph(str)`: word-wrapping multi-line text, plus `paragraphAlignLeft`, `paragraphAlignCenter`, `paragraphAlignRight`, `paragraphAlignJustify`.

```cpp
auto body = paragraph("Long text wraps automatically to fit the container width.")
            | borderRounded;
```

See the live [`dom/paragraph`](/examples/dom/paragraph) and [`dom/html_like`](/examples/dom/html_like) examples.

## Layout Containers

| Function | Layout |
| --- | --- |
| `hbox({...})` | Horizontal row |
| `vbox({...})` | Vertical column |
| `dbox({...})` | Stacked layers (depth box; pair with `clear_under`) |
| `flexbox({...}, config)` | CSS-like wrapping layout (`hflow` / `vflow` shorthands) |
| `gridbox({ {...}, {...} })` | 2D aligned grid |
| `filler()` | Expandable empty space |

Use `flex` (or `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `size(WIDTH, EQUAL, n)`) to control how elements share available space:

```cpp
auto left = text("left") | border;
auto middle = text("middle") | border | flex;
auto right = text("right") | border;

auto row = hbox({left, middle, right});
```

```text
┌────┐┌─────────────────────────────────────────────────────┐┌─────┐
│left││middle                                               ││right│
└────┘└─────────────────────────────────────────────────────┘└─────┘
```

Explore live layout demos: [`dom/vbox_hbox`](/examples/dom/vbox_hbox), [`dom/gridbox`](/examples/dom/gridbox), [`dom/hflow`](/examples/dom/hflow), [`dom/dbox`](/examples/dom/dbox), [`dom/size`](/examples/dom/size).

## Borders, Windows & Separators

- **Borders:** `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(BorderStyle, Color)`.
- **`window(title, content)`:** Bordered box with a title embedded in the top edge.
- **Separators:** `separator()`, `separatorLight()`, `separatorDouble()`, `separatorHeavy()`, `separatorDashed()`, `separatorEmpty()`, `separatorCharacter(str)`.

```cpp
auto card = window(text(" Status ") | bold,
                   vbox({
                       text("CPU: 12%"),
                       separator(),
                       text("RAM: 4.2 GB"),
                   }));
```

See [`dom/border_style`](/examples/dom/border_style), [`dom/border_colored`](/examples/dom/border_colored), and [`dom/separator_style`](/examples/dom/separator_style).

## Colors, Gradients & Text Styles

FTXUI supports 16-color (`Color::Red`, `Color::BlueLight`, …), 256-color (`Color::Palette256`), and 24-bit TrueColor (`Color::RGB(r, g, b)`, `Color::HSV(h, s, v)`), automatically downgrading on terminals with limited color support.

```cpp
auto styled = text("Hello FTXUI")
              | bold
              | underlined
              | color(Color::RGB(120, 220, 255))
              | bgcolor(Color::RGB(20, 25, 40));

auto gradient_box = text("Gradient")
                    | center
                    | bgcolor(LinearGradient(45, Color::DeepPink1, Color::DodgerBlue1));
```

<FtxuiDemo file="dom/color_gallery" height="440px" title="Live demo: dom/color_gallery" />

Text style decorators: `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough`, `hyperlink(url)`.

See [`dom/style_gallery`](/examples/dom/style_gallery), [`dom/linear_gradient`](/examples/dom/linear_gradient), and [`dom/color_info_palette256`](/examples/dom/color_info_palette256).

## Gauges, Spinners & Graphs

- `gauge(progress)` (`0.0f` to `1.0f`), plus `gaugeLeft`, `gaugeRight`, `gaugeUp`, `gaugeDown`.
- `spinner(charset_index, frame)`: 23 built-in animated spinner styles.
- `graph(fn)`: renders a `std::vector<int>(int width, int height)` callback as a Braille sparkline.

See [`dom/gauge`](/examples/dom/gauge), [`dom/gauge_direction`](/examples/dom/gauge_direction), [`dom/spinner`](/examples/dom/spinner), and [`dom/graph`](/examples/dom/graph).

## Tables

Include `<ftxui/dom/table.hpp>` to build aligned tables and style rows, columns, or rectangles with `TableSelection`:

```cpp
#include <ftxui/dom/table.hpp>

auto table = Table({
    {"Language", "Stars", "License"},
    {"C++",      "10k+",  "MIT"},
    {"FTXUI",    "Fast",  "MIT"},
});

table.SelectAll().Border(LIGHT);
table.SelectRow(0).Decorate(bold);
table.SelectRow(0).SeparatorVertical(LIGHT);
table.SelectRow(0).Border(DOUBLE);
table.SelectColumn(1).DecorateCells(align_right);

auto doc = table.Render();
```

See [`dom/table`](/examples/dom/table) for the full styling showcase.

## Canvas

Include `<ftxui/dom/canvas.hpp>` for sub-cell pixel/Braille/block drawing (lines, circles, text, points):

```cpp
#include <ftxui/dom/canvas.hpp>

auto c = Canvas(80, 40);
c.DrawPointCircle(40, 20, 15, Color::Cyan);
c.DrawPointLine(0, 0, 80, 40, Color::Yellow);
c.DrawText(28, 20, "FTXUI", Color::White);

auto doc = canvas(std::move(c)) | border;
```

See [`dom/canvas`](/examples/dom/canvas) and [`component/canvas_animated`](/examples/component/canvas_animated).

## Scrolling, Reflection & Layering

- `frame`, `xframe`, `yframe`: clip an element to a viewport scrolled to whichever child has `focus`. Pair with `vscroll_indicator` / `hscroll_indicator`.
- `reflect(box)`: records the element's final screen coordinates into a `Box` (useful for mouse hit-testing).
- `clear_under`: clears cells behind an overlay inside a `dbox`.
- `automerge`: merges adjacent box-drawing border characters (`─`, `│`, `┌`, `┼`).

For the full list of DOM functions and signatures, see the [`ftxui/dom` API Reference](/api/dom).
