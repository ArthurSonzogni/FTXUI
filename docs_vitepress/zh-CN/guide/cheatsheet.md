# 速查表

日常开发 FTXUI 应用的一页式速查参考。

## 常用头文件

```cpp
#include <ftxui/screen/screen.hpp>       // Screen, Surface, Cell, Color, Box
#include <ftxui/dom/elements.hpp>        // hbox, vbox, text, border, flex, ...
#include <ftxui/dom/table.hpp>           // Table
#include <ftxui/dom/canvas.hpp>          // Canvas
#include <ftxui/component/component.hpp> // Button, Input, Menu, Container, ...
#include <ftxui/component/app.hpp>       // App
#include <ftxui/component/loop.hpp>      // Loop (manual event loop)
```

## 布局元素 (`ftxui/dom`)

| 元素 | 用途 |
| --- | --- |
| `hbox({...})` / `vbox({...})` | 水平 / 垂直盒子布局 |
| `dbox({...})` | 叠层布局（深度盒子，通常配合 `clear_under` 使用） |
| `flexbox({...}, config)` | 类 CSS 自动换行弹性布局（简写：`hflow`、`vflow`） |
| `gridbox({ {...}, {...} })` | 二维对齐网格 |
| `filler()` / `emptyElement()` | 可伸缩空白区域 / 0×0 空元素 |
| `separator()` | 水平或垂直分隔线（`separatorLight`、`Double`、`Heavy`、`Dashed`） |
| `window(title, content)` | 带标题栏的边框窗口 |

## 装饰器（在左侧用 `|` 链式拼接）

```cpp
auto el = text("Hello")
          | bold
          | color(Color::Cyan)
          | bgcolor(Color::Black)
          | borderRounded
          | center
          | flex;
```

| 分类 | 装饰器 |
| --- | --- |
| **文本样式** | `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough` |
| **颜色** | `color(Color::Red)`, `bgcolor(Color::Blue)`, `color(LinearGradient(...))` |
| **边框** | `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(...)` |
| **弹性与尺寸** | `flex`, `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `notflex`, `size(WIDTH, EQUAL, 20)`（`LESS_THAN`, `GREATER_THAN`） |
| **对齐** | `center`, `hcenter`, `vcenter`, `align_right` |
| **滚动** | `frame`, `xframe`, `yframe`, `vscroll_indicator`, `hscroll_indicator`, `focus`, `focusPositionRelative(x, y)` |
| **实用工具** | `clear_under`, `reflect(box)`, `automerge`, `hyperlink(url)` |

## 交互式组件 (`ftxui/component`)

| 组件 | 典型用法 |
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
| `ResizableSplit` | `ResizableSplitLeft(left, right, &size)`（`Right`, `Top`, `Bottom`） |
| `Container` | `Container::Vertical({...})`, `Horizontal`, `Tab({...}, &idx)`, `Stacked` |
| `Renderer` | `Renderer(container, [&] { return vbox({...}); })` |
| `CatchEvent` | `comp \| CatchEvent([&](Event e) { return false; })` |
| `Maybe` | `comp \| Maybe(&visible)` |
| `Hoverable` | `comp \| Hoverable(&hovered)` |

## 常用事件 (`ftxui::Event`)

| 按键/输入 | 事件 |
| --- | --- |
| 可打印字符 | `Event::Character('q')`, `event.is_character()`, `event.character()` |
| 方向键 | `Event::ArrowUp`, `ArrowDown`, `ArrowLeft`, `ArrowRight`（`*Ctrl`, `*Alt`） |
| 导航键 | `Event::Tab`, `TabReverse`, `Home`, `End`, `PageUp`, `PageDown` |
| 编辑键 | `Event::Return`, `Escape`, `Backspace`, `Delete`, `Insert` |
| 控制键与功能键 | `Event::CtrlC`, `CtrlD`, `CtrlA`..`CtrlZ`, `AltA`..`AltZ`, `F1`..`F12` |
| 鼠标 | `event.is_mouse()`, `event.mouse().button`（`Mouse::Left`, `WheelUp`, `WheelDown`）, `event.mouse().motion`（`Pressed`, `Released`, `Moved`）, `x`, `y` |

## `App` 生命周期

```cpp
auto app = App::Fullscreen();      // or TerminalOutput(), FitComponent(), FixedSize(w, h)
app.Loop(root_component);          // Blocking main loop
app.Exit();                        // Exit loop (or app.ExitLoopClosure())
app.Post([&] { /* state */ });     // Thread-safe callback on main loop thread
app.RequestAnimationFrame();       // Schedule a redraw
app.PrintAbove(element);           // Permanent scrollback output (TerminalOutput mode)
app.WithRestoredIO([&] { ... });   // Suspend raw mode to run external CLI/editor
```
