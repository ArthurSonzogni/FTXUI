# Chuleta de referencia

Referencia rápida de una sola página para el desarrollo diario con FTXUI.

## Cabeceras

```cpp
#include <ftxui/screen/screen.hpp>       // Screen, Surface, Cell, Color, Box
#include <ftxui/dom/elements.hpp>        // hbox, vbox, text, border, flex, ...
#include <ftxui/dom/table.hpp>           // Table
#include <ftxui/dom/canvas.hpp>          // Canvas
#include <ftxui/component/component.hpp> // Button, Input, Menu, Container, ...
#include <ftxui/component/app.hpp>       // App
#include <ftxui/component/loop.hpp>      // Loop (manual event loop)
```

## Elementos de diseño (`ftxui/dom`)

| Elemento | Propósito |
| --- | --- |
| `hbox({...})` / `vbox({...})` | Caja horizontal / vertical |
| `dbox({...})` | Capas superpuestas (caja de profundidad; combinar con `clear_under`) |
| `flexbox({...}, config)` | Diseño con ajuste de línea estilo CSS (atajos `hflow`, `vflow`) |
| `gridbox({ {...}, {...} })` | Cuadrícula 2D alineada |
| `filler()` / `emptyElement()` | Espacio en blanco expandible / elemento de 0×0 |
| `separator()` | Divisor horizontal o vertical (`separatorLight`, `Double`, `Heavy`, `Dashed`) |
| `window(title, content)` | Caja con borde y barra de título |

## Decoradores (encadenar con `|` a la izquierda)

```cpp
auto el = text("Hello")
          | bold
          | color(Color::Cyan)
          | bgcolor(Color::Black)
          | borderRounded
          | center
          | flex;
```

| Categoría | Decoradores |
| --- | --- |
| **Estilo de texto** | `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough` |
| **Color** | `color(Color::Red)`, `bgcolor(Color::Blue)`, `color(LinearGradient(...))` |
| **Bordes** | `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(...)` |
| **Flex y tamaño** | `flex`, `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `notflex`, `size(WIDTH, EQUAL, 20)` (`LESS_THAN`, `GREATER_THAN`) |
| **Alineación** | `center`, `hcenter`, `vcenter`, `align_right` |
| **Desplazamiento** | `frame`, `xframe`, `yframe`, `vscroll_indicator`, `hscroll_indicator`, `focus`, `focusPositionRelative(x, y)` |
| **Utilidades** | `clear_under`, `reflect(box)`, `automerge`, `hyperlink(url)` |

## Componentes interactivos (`ftxui/component`)

| Componente | Uso típico |
| --- | --- |
| `Button` | `auto btn = Button("Click", [&] { ... });` |
| `Input` | `auto in = Input(&str, "placeholder");` |
| `Checkbox` | `auto cb = Checkbox("Enable", &flag);` |
| `Menu` | `auto m = Menu(&entries, &selected);` |
| `Radiobox` | `auto rb = Radiobox(&entries, &selected);` |
| `Dropdown` | `auto dd = Dropdown(&entries, &selected);` |
| `Toggle` | `auto tg = Toggle(&entries, &selected);` |
| `Slider` | `auto sl = Slider("Volume:", &val, 0, 100, 5);` |
| `Collapsible` | `auto col = Collapsible("Details", child);` |
| `Modal` | `auto ui = main \| Modal(dialog, &show);` |
| `Window` | `auto win = Window({.inner = child, .title = "Title"});` |
| `ResizableSplit` | `ResizableSplitLeft(left, right, &size)` (`Right`, `Top`, `Bottom`) |
| `Container` | `Container::Vertical({...})`, `Horizontal`, `Tab({...}, &idx)`, `Stacked` |
| `Renderer` | `Renderer(container, [&] { return vbox({...}); })` |
| `CatchEvent` | `comp \| CatchEvent([&](Event e) { return false; })` |
| `Maybe` | `comp \| Maybe(&visible)` |
| `Hoverable` | `comp \| Hoverable(&hovered)` |

## Eventos comunes (`ftxui::Event`)

| Teclas | Evento |
| --- | --- |
| Carácter imprimible | `Event::Character('q')`, `event.is_character()`, `event.character()` |
| Flechas | `Event::ArrowUp`, `ArrowDown`, `ArrowLeft`, `ArrowRight` (`*Ctrl`, `*Alt`) |
| Navegación | `Event::Tab`, `TabReverse`, `Home`, `End`, `PageUp`, `PageDown` |
| Edición | `Event::Return`, `Escape`, `Backspace`, `Delete`, `Insert` |
| Control | `Event::CtrlC`, `CtrlD`, `CtrlA`..`CtrlZ`, `AltA`..`AltZ`, `F1`..`F12` |
| Ratón | `event.is_mouse()`, `event.mouse().button` (`Mouse::Left`, `WheelUp`, `WheelDown`), `event.mouse().motion` (`Pressed`, `Released`, `Moved`), `x`, `y` |

## Ciclo de vida de `App`

```cpp
auto app = App::Fullscreen();      // or TerminalOutput(), FitComponent(), FixedSize(w, h)
app.Loop(root_component);          // Blocking main loop
app.Exit();                        // Exit loop (or app.ExitLoopClosure())
app.Post([&] { /* state */ });     // Thread-safe callback on main loop thread
app.RequestAnimationFrame();       // Schedule a redraw
app.PrintAbove(element);           // Permanent scrollback output (TerminalOutput mode)
app.WithRestoredIO([&] { ... });   // Suspend raw mode to run external CLI/editor
```
