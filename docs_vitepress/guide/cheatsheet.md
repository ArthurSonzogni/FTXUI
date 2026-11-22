# Cheat Sheet

A 1-page quick reference for everyday FTXUI development.

## Headers

```cpp
#include <ftxui/screen/screen.hpp>       // Screen, Surface, Cell, Color, Box
#include <ftxui/dom/elements.hpp>        // hbox, vbox, text, border, flex, ...
#include <ftxui/dom/table.hpp>           // Table
#include <ftxui/dom/canvas.hpp>          // Canvas
#include <ftxui/component/component.hpp> // Button, Input, Menu, Container, ...
#include <ftxui/component/app.hpp>       // App
#include <ftxui/component/loop.hpp>      // Loop (manual event loop)
```

## Layout Elements (`ftxui/dom`)

| Element | Purpose |
| --- | --- |
| `hbox({...})` / `vbox({...})` | Horizontal / vertical box |
| `dbox({...})` | Stacked layers (depth box; pair with `clear_under`) |
| `flexbox({...}, config)` | CSS-like wrapping layout (`hflow`, `vflow` shorthands) |
| `gridbox({ {...}, {...} })` | 2D aligned grid |
| `filler()` / `emptyElement()` | Expandable blank space / 0×0 element |
| `separator()` | Horizontal or vertical divider (`separatorLight`, `Double`, `Heavy`, `Dashed`) |
| `window(title, content)` | Bordered box with title bar |

## Decorators (chain with `|` on the left)

```cpp
auto el = text("Hello")
          | bold
          | color(Color::Cyan)
          | bgcolor(Color::Black)
          | borderRounded
          | center
          | flex;
```

| Category | Decorators |
| --- | --- |
| **Text style** | `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough` |
| **Color** | `color(Color::Red)`, `bgcolor(Color::Blue)`, `color(LinearGradient(...))` |
| **Borders** | `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(...)` |
| **Flex & Size** | `flex`, `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `notflex`, `size(WIDTH, EQUAL, 20)` (`LESS_THAN`, `GREATER_THAN`) |
| **Alignment** | `center`, `hcenter`, `vcenter`, `align_right` |
| **Scrolling** | `frame`, `xframe`, `yframe`, `vscroll_indicator`, `hscroll_indicator`, `focus`, `focusPositionRelative(x, y)` |
| **Utilities** | `clear_under`, `reflect(box)`, `automerge`, `hyperlink(url)` |

## Interactive Components (`ftxui/component`)

| Component | Typical Usage |
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

## Common Events (`ftxui::Event`)

| Keys | Event |
| --- | --- |
| Printable char | `Event::Character('q')`, `event.is_character()`, `event.character()` |
| Arrows | `Event::ArrowUp`, `ArrowDown`, `ArrowLeft`, `ArrowRight` (`*Ctrl`, `*Alt`) |
| Navigation | `Event::Tab`, `TabReverse`, `Home`, `End`, `PageUp`, `PageDown` |
| Editing | `Event::Return`, `Escape`, `Backspace`, `Delete`, `Insert` |
| Control | `Event::CtrlC`, `CtrlD`, `CtrlA`..`CtrlZ`, `AltA`..`AltZ`, `F1`..`F12` |
| Mouse | `event.is_mouse()`, `event.mouse().button` (`Mouse::Left`, `WheelUp`, `WheelDown`), `event.mouse().motion` (`Pressed`, `Released`, `Moved`), `x`, `y` |

## `App` Lifecycle

```cpp
auto app = App::Fullscreen();      // or TerminalOutput(), FitComponent(), FixedSize(w, h)
app.Loop(root_component);          // Blocking main loop
app.Exit();                        // Exit loop (or app.ExitLoopClosure())
app.Post([&] { /* state */ });     // Thread-safe callback on main loop thread
app.RequestAnimationFrame();       // Schedule a redraw
app.PrintAbove(element);           // Permanent scrollback output (TerminalOutput mode)
app.WithRestoredIO([&] { ... });   // Suspend raw mode to run external CLI/editor
```
