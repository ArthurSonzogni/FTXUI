# Módulo DOM (`ftxui/dom`)

El módulo **DOM** define un árbol declarativo y sin estado de nodos `Element` que ajustan su diseño de forma responsiva al tamaño de la terminal.

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

Los decoradores se pueden aplicar de tres formas equivalentes:
- Llamada a función: `document = border(document);`
- Operador pipe: `document = document | border;`
- Pipe en el sitio: `document |= border;`

## Texto y párrafos

- `text(str)`: texto UTF-8 de una sola línea (compatible con caracteres de ancho completo, CJK y emojis).
- `vtext(str)`: texto vertical (un grafema por fila).
- `paragraph(str)`: texto multilínea con ajuste de palabras, además de `paragraphAlignLeft`, `paragraphAlignCenter`, `paragraphAlignRight` y `paragraphAlignJustify`.

```cpp
auto body = paragraph("Long text wraps automatically to fit the container width.")
            | borderRounded;
```

Consulta los ejemplos en vivo [`dom/paragraph`](/examples/dom/paragraph) y [`dom/html_like`](/examples/dom/html_like).

## Contenedores de diseño

| Función | Diseño |
| --- | --- |
| `hbox({...})` | Fila horizontal |
| `vbox({...})` | Columna vertical |
| `dbox({...})` | Capas superpuestas (caja de profundidad; combinar con `clear_under`) |
| `flexbox({...}, config)` | Diseño con ajuste estilo CSS (atajos `hflow` / `vflow`) |
| `gridbox({ {...}, {...} })` | Cuadrícula 2D alineada |
| `filler()` | Espacio vacío expandible |

Usa `flex` (o `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `size(WIDTH, EQUAL, n)`) para controlar cómo se reparten el espacio disponible los elementos:

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

Explora las demos de diseño en vivo: [`dom/vbox_hbox`](/examples/dom/vbox_hbox), [`dom/gridbox`](/examples/dom/gridbox), [`dom/hflow`](/examples/dom/hflow), [`dom/dbox`](/examples/dom/dbox) y [`dom/size`](/examples/dom/size).

## Bordes, ventanas y separadores

- **Bordes:** `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(BorderStyle, Color)`.
- **`window(title, content)`:** caja con borde y un título integrado en el borde superior.
- **Separadores:** `separator()`, `separatorLight()`, `separatorDouble()`, `separatorHeavy()`, `separatorDashed()`, `separatorEmpty()`, `separatorCharacter(str)`.

```cpp
auto card = window(text(" Status ") | bold,
                   vbox({
                       text("CPU: 12%"),
                       separator(),
                       text("RAM: 4.2 GB"),
                   }));
```

Consulta [`dom/border_style`](/examples/dom/border_style), [`dom/border_colored`](/examples/dom/border_colored) y [`dom/separator_style`](/examples/dom/separator_style).

## Colores, degradados y estilos de texto

FTXUI soporta 16 colores (`Color::Red`, `Color::BlueLight`, …), 256 colores (`Color::Palette256`) y TrueColor de 24 bits (`Color::RGB(r, g, b)`, `Color::HSV(h, s, v)`), degradando automáticamente en terminales con soporte de color limitado.

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

Decoradores de estilo de texto: `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough`, `hyperlink(url)`.

Consulta [`dom/style_gallery`](/examples/dom/style_gallery), [`dom/linear_gradient`](/examples/dom/linear_gradient) y [`dom/color_info_palette256`](/examples/dom/color_info_palette256).

## Medidores, indicadores giratorios y gráficas

- `gauge(progress)` (de `0.0f` a `1.0f`), además de `gaugeLeft`, `gaugeRight`, `gaugeUp`, `gaugeDown`.
- `spinner(charset_index, frame)`: 23 estilos animados integrados.
- `graph(fn)`: renderiza un callback `std::vector<int>(int width, int height)` como una gráfica compacta en Braille.

Consulta [`dom/gauge`](/examples/dom/gauge), [`dom/gauge_direction`](/examples/dom/gauge_direction), [`dom/spinner`](/examples/dom/spinner) y [`dom/graph`](/examples/dom/graph).

## Tablas

Incluye `<ftxui/dom/table.hpp>` para construir tablas alineadas y dar estilo a filas, columnas o rectángulos con `TableSelection`:

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

Consulta [`dom/table`](/examples/dom/table) para ver la galería completa de estilos.

## Canvas

Incluye `<ftxui/dom/canvas.hpp>` para dibujar a nivel subcelda con píxeles, Braille o bloques (líneas, círculos, texto, puntos):

```cpp
#include <ftxui/dom/canvas.hpp>

auto c = Canvas(80, 40);
c.DrawPointCircle(40, 20, 15, Color::Cyan);
c.DrawPointLine(0, 0, 80, 40, Color::Yellow);
c.DrawText(28, 20, "FTXUI", Color::White);

auto doc = canvas(std::move(c)) | border;
```

Consulta [`dom/canvas`](/examples/dom/canvas) y [`component/canvas_animated`](/examples/component/canvas_animated).

## Desplazamiento, reflexión y capas

- `frame`, `xframe`, `yframe`: recortan un elemento a una ventana desplazada hacia el hijo que tenga `focus`. Combínalo con `vscroll_indicator` / `hscroll_indicator`.
- `reflect(box)`: registra las coordenadas finales en pantalla del elemento en un `Box` (útil para detectar clics de ratón).
- `clear_under`: limpia las celdas situadas debajo de una capa superpuesta dentro de un `dbox`.
- `automerge`: fusiona caracteres de borde adyacentes (`─`, `│`, `┌`, `┼`).

Para la lista completa de funciones y firmas del DOM, consulta la [Referencia de la API de `ftxui/dom`](/api/dom).
