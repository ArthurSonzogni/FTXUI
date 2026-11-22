# 速查表

日常開發 FTXUI 應用程式的一頁式快速參考指南。

## 標頭檔

```cpp
#include <ftxui/screen/screen.hpp>       // Screen, Surface, Cell, Color, Box
#include <ftxui/dom/elements.hpp>        // hbox, vbox, text, border, flex, ...
#include <ftxui/dom/table.hpp>           // Table
#include <ftxui/dom/canvas.hpp>          // Canvas
#include <ftxui/component/component.hpp> // Button, Input, Menu, Container, ...
#include <ftxui/component/app.hpp>       // App
#include <ftxui/component/loop.hpp>      // Loop (manual event loop)
```

## 版面配置元素（`ftxui/dom`）

| 元素 | 用途 |
| --- | --- |
| `hbox({...})` / `vbox({...})` | 水平／垂直排列容器 |
| `dbox({...})` | 層疊容器（深度排列；常搭配 `clear_under` 使用） |
| `flexbox({...}, config)` | 類似 CSS 的自動換行排版（另有 `hflow`、`vflow` 簡寫） |
| `gridbox({ {...}, {...} })` | 二維對齊網格 |
| `filler()` / `emptyElement()` | 可延展的空白區域／0×0 空元素 |
| `separator()` | 水平或垂直分隔線（`separatorLight`、`Double`、`Heavy`、`Dashed`） |
| `window(title, content)` | 帶有標題列的邊框方塊 |

## 裝飾器（在右側以 `|` 串接）

```cpp
auto el = text("Hello")
          | bold
          | color(Color::Cyan)
          | bgcolor(Color::Black)
          | borderRounded
          | center
          | flex;
```

| 分類 | 裝飾器 |
| --- | --- |
| **文字樣式** | `bold`、`dim`、`italic`、`inverted`、`underlined`、`underlinedDouble`、`blink`、`strikethrough` |
| **色彩** | `color(Color::Red)`、`bgcolor(Color::Blue)`、`color(LinearGradient(...))` |
| **邊框** | `border`、`borderLight`、`borderHeavy`、`borderDouble`、`borderRounded`、`borderDashed`、`borderEmpty`、`borderStyled(...)` |
| **彈性與尺寸** | `flex`、`flex_grow`、`flex_shrink`、`xflex`、`yflex`、`notflex`、`size(WIDTH, EQUAL, 20)`（另有 `LESS_THAN`、`GREATER_THAN`） |
| **對齊** | `center`、`hcenter`、`vcenter`、`align_right` |
| **捲動** | `frame`、`xframe`、`yframe`、`vscroll_indicator`、`hscroll_indicator`、`focus`、`focusPositionRelative(x, y)` |
| **工具** | `clear_under`、`reflect(box)`、`automerge`、`hyperlink(url)` |

## 互動式元件（`ftxui/component`）

| 元件 | 常見用法 |
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
| `ResizableSplit` | `ResizableSplitLeft(left, right, &size)`（另有 `Right`、`Top`、`Bottom`） |
| `Container` | `Container::Vertical({...})`、`Horizontal`、`Tab({...}, &idx)`、`Stacked` |
| `Renderer` | `Renderer(container, [&] { return vbox({...}); })` |
| `CatchEvent` | `comp \| CatchEvent([&](Event e) { return false; })` |
| `Maybe` | `comp \| Maybe(&visible)` |
| `Hoverable` | `comp \| Hoverable(&hovered)` |

## 常見事件（`ftxui::Event`）

| 按鍵 | 事件 |
| --- | --- |
| 可列印字元 | `Event::Character('q')`、`event.is_character()`、`event.character()` |
| 方向鍵 | `Event::ArrowUp`、`ArrowDown`、`ArrowLeft`、`ArrowRight`（另有 `*Ctrl`、`*Alt`） |
| 導覽鍵 | `Event::Tab`、`TabReverse`、`Home`、`End`、`PageUp`、`PageDown` |
| 編輯鍵 | `Event::Return`、`Escape`、`Backspace`、`Delete`、`Insert` |
| 控制鍵 | `Event::CtrlC`、`CtrlD`、`CtrlA`..`CtrlZ`、`AltA`..`AltZ`、`F1`..`F12` |
| 滑鼠 | `event.is_mouse()`、`event.mouse().button`（`Mouse::Left`、`WheelUp`、`WheelDown`）、`event.mouse().motion`（`Pressed`、`Released`、`Moved`）、`x`、`y` |

## `App` 生命週期

```cpp
auto app = App::Fullscreen();      // or TerminalOutput(), FitComponent(), FixedSize(w, h)
app.Loop(root_component);          // Blocking main loop
app.Exit();                        // Exit loop (or app.ExitLoopClosure())
app.Post([&] { /* state */ });     // Thread-safe callback on main loop thread
app.RequestAnimationFrame();       // Schedule a redraw
app.PrintAbove(element);           // Permanent scrollback output (TerminalOutput mode)
app.WithRestoredIO([&] { ... });   // Suspend raw mode to run external CLI/editor
```
