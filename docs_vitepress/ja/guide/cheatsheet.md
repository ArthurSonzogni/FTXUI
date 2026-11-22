# チートシート

日々の FTXUI 開発で参照できる1ページ早見表です。

## ヘッダ

```cpp
#include <ftxui/screen/screen.hpp>       // Screen, Surface, Cell, Color, Box
#include <ftxui/dom/elements.hpp>        // hbox, vbox, text, border, flex, ...
#include <ftxui/dom/table.hpp>           // Table
#include <ftxui/dom/canvas.hpp>          // Canvas
#include <ftxui/component/component.hpp> // Button, Input, Menu, Container, ...
#include <ftxui/component/app.hpp>       // App
#include <ftxui/component/loop.hpp>      // Loop (manual event loop)
```

## レイアウト要素 (`ftxui/dom`)

| 要素 | 用途 |
| --- | --- |
| `hbox({...})` / `vbox({...})` | 水平 / 垂直ボックス |
| `dbox({...})` | 重ね合わせレイヤー（深度ボックス。`clear_under` と併用） |
| `flexbox({...}, config)` | CSS 風の折り返しレイアウト（省略形として `hflow`, `vflow`） |
| `gridbox({ {...}, {...} })` | 2次元グリッド配置 |
| `filler()` / `emptyElement()` | 伸縮する空白 / 0×0 要素 |
| `separator()` | 水平または垂直の区切り線（`separatorLight`, `Double`, `Heavy`, `Dashed`） |
| `window(title, content)` | タイトルバー付きの枠線ボックス |

## デコレータ（左から `|` で連結）

```cpp
auto el = text("Hello")
          | bold
          | color(Color::Cyan)
          | bgcolor(Color::Black)
          | borderRounded
          | center
          | flex;
```

| カテゴリ | デコレータ |
| --- | --- |
| **テキストスタイル** | `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough` |
| **色** | `color(Color::Red)`, `bgcolor(Color::Blue)`, `color(LinearGradient(...))` |
| **枠線** | `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(...)` |
| **伸縮・サイズ** | `flex`, `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `notflex`, `size(WIDTH, EQUAL, 20)`（`LESS_THAN`, `GREATER_THAN`） |
| **配置** | `center`, `hcenter`, `vcenter`, `align_right` |
| **スクロール** | `frame`, `xframe`, `yframe`, `vscroll_indicator`, `hscroll_indicator`, `focus`, `focusPositionRelative(x, y)` |
| **ユーティリティ** | `clear_under`, `reflect(box)`, `automerge`, `hyperlink(url)` |

## インタラクティブコンポーネント (`ftxui/component`)

| コンポーネント | 代表的な使い方 |
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

## よく使うイベント (`ftxui::Event`)

| キー / 操作 | イベント |
| --- | --- |
| 印字可能文字 | `Event::Character('q')`, `event.is_character()`, `event.character()` |
| 矢印キー | `Event::ArrowUp`, `ArrowDown`, `ArrowLeft`, `ArrowRight`（`*Ctrl`, `*Alt`） |
| ナビゲーション | `Event::Tab`, `TabReverse`, `Home`, `End`, `PageUp`, `PageDown` |
| 編集 | `Event::Return`, `Escape`, `Backspace`, `Delete`, `Insert` |
| 制御・ファンクション | `Event::CtrlC`, `CtrlD`, `CtrlA`..`CtrlZ`, `AltA`..`AltZ`, `F1`..`F12` |
| マウス | `event.is_mouse()`, `event.mouse().button`（`Mouse::Left`, `WheelUp`, `WheelDown`）, `event.mouse().motion`（`Pressed`, `Released`, `Moved`）, `x`, `y` |

## `App` ライフサイクル

```cpp
auto app = App::Fullscreen();      // or TerminalOutput(), FitComponent(), FixedSize(w, h)
app.Loop(root_component);          // Blocking main loop
app.Exit();                        // Exit loop (or app.ExitLoopClosure())
app.Post([&] { /* state */ });     // Thread-safe callback on main loop thread
app.RequestAnimationFrame();       // Schedule a redraw
app.PrintAbove(element);           // Permanent scrollback output (TerminalOutput mode)
app.WithRestoredIO([&] { ... });   // Suspend raw mode to run external CLI/editor
```
