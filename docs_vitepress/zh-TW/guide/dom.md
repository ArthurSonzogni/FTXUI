# DOM 模組（`ftxui/dom`）

**DOM** 模組定義了一套宣告式、無狀態的 `Element` 節點樹，能夠依據終端機視窗大小自動進行響應式排版。

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

套用裝飾器有三種完全等價的寫法：
- 函式呼叫：`document = border(document);`
- 管線運算子：`document = document | border;`
- 原地管線運算子：`document |= border;`

## 文字與段落

- `text(str)`：單行 UTF-8 文字（支援全形字、中日韓文字與 emoji）。
- `vtext(str)`：垂直排列文字（每列顯示一個字素）。
- `paragraph(str)`：會自動換行的多行段落文字，另有 `paragraphAlignLeft`、`paragraphAlignCenter`、`paragraphAlignRight`、`paragraphAlignJustify`。

```cpp
auto body = paragraph("Long text wraps automatically to fit the container width.")
            | borderRounded;
```

可參閱線上範例 [`dom/paragraph`](/examples/dom/paragraph) 與 [`dom/html_like`](/examples/dom/html_like)。

## 版面配置容器

| 函式 | 排版方式 |
| --- | --- |
| `hbox({...})` | 水平列 |
| `vbox({...})` | 垂直欄 |
| `dbox({...})` | 層疊容器（深度排列；常搭配 `clear_under` 使用） |
| `flexbox({...}, config)` | 類似 CSS 的自動換行排版（另有 `hflow` / `vflow` 簡寫） |
| `gridbox({ {...}, {...} })` | 二維對齊網格 |
| `filler()` | 可延展的空白區域 |

使用 `flex`（或 `flex_grow`、`flex_shrink`、`xflex`、`yflex`、`size(WIDTH, EQUAL, n)`）來控制元素如何分配可用空間：

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

探索線上排版範例：[`dom/vbox_hbox`](/examples/dom/vbox_hbox)、[`dom/gridbox`](/examples/dom/gridbox)、[`dom/hflow`](/examples/dom/hflow)、[`dom/dbox`](/examples/dom/dbox)、[`dom/size`](/examples/dom/size)。

## 邊框、視窗與分隔線

- **邊框**：`border`、`borderLight`、`borderHeavy`、`borderDouble`、`borderRounded`、`borderDashed`、`borderEmpty`、`borderStyled(BorderStyle, Color)`。
- **`window(title, content)`**：在頂部邊框嵌入標題的方塊。
- **分隔線**：`separator()`、`separatorLight()`、`separatorDouble()`、`separatorHeavy()`、`separatorDashed()`、`separatorEmpty()`、`separatorCharacter(str)`。

```cpp
auto card = window(text(" Status ") | bold,
                   vbox({
                       text("CPU: 12%"),
                       separator(),
                       text("RAM: 4.2 GB"),
                   }));
```

參閱 [`dom/border_style`](/examples/dom/border_style)、[`dom/border_colored`](/examples/dom/border_colored) 與 [`dom/separator_style`](/examples/dom/separator_style)。

## 色彩、漸層與文字樣式

FTXUI 支援 16 色（`Color::Red`、`Color::BlueLight` 等）、256 色（`Color::Palette256`）與 24 位元 TrueColor（`Color::RGB(r, g, b)`、`Color::HSV(h, s, v)`），在色彩支援較有限的終端機上會自動降級顯示。

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

文字樣式裝飾器：`bold`、`dim`、`italic`、`inverted`、`underlined`、`underlinedDouble`、`blink`、`strikethrough`、`hyperlink(url)`。

參閱 [`dom/style_gallery`](/examples/dom/style_gallery)、[`dom/linear_gradient`](/examples/dom/linear_gradient) 與 [`dom/color_info_palette256`](/examples/dom/color_info_palette256)。

## 進度條、載入動畫與折線圖

- `gauge(progress)`（`0.0f` 到 `1.0f`），另有 `gaugeLeft`、`gaugeRight`、`gaugeUp`、`gaugeDown`。
- `spinner(charset_index, frame)`：內建 23 種動畫載入圖示樣式。
- `graph(fn)`：將 `std::vector<int>(int width, int height)` 回呼函式的輸出渲染為點字（Braille）走勢圖。

參閱 [`dom/gauge`](/examples/dom/gauge)、[`dom/gauge_direction`](/examples/dom/gauge_direction)、[`dom/spinner`](/examples/dom/spinner) 與 [`dom/graph`](/examples/dom/graph)。

## 表格

引入 `<ftxui/dom/table.hpp>` 即可建立對齊表格，並透過 `TableSelection` 為特定列、欄或矩形區域套用樣式：

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

完整的樣式展示請見 [`dom/table`](/examples/dom/table)。

## 畫布（Canvas）

引入 `<ftxui/dom/canvas.hpp>` 可進行次字元格等級的像素／點字／方塊繪圖（線條、圓形、文字、點）：

```cpp
#include <ftxui/dom/canvas.hpp>

auto c = Canvas(80, 40);
c.DrawPointCircle(40, 20, 15, Color::Cyan);
c.DrawPointLine(0, 0, 80, 40, Color::Yellow);
c.DrawText(28, 20, "FTXUI", Color::White);

auto doc = canvas(std::move(c)) | border;
```

參閱 [`dom/canvas`](/examples/dom/canvas) 與 [`component/canvas_animated`](/examples/component/canvas_animated)。

## 捲動、座標反射與圖層疊加

- `frame`、`xframe`、`yframe`：將元素裁切至可視範圍，並自動捲動至擁有 `focus` 的子元素。常搭配 `vscroll_indicator` / `hscroll_indicator` 使用。
- `reflect(box)`：將元素最終在畫面上的座標記錄到 `Box` 中（常用於滑鼠點擊偵測）。
- `clear_under`：清除 `dbox` 中覆蓋層底下的字元格內容。
- `automerge`：自動合併相鄰的方框繪製線條字元（`─`、`│`、`┌`、`┼`）。

完整的 DOM 函式列表與簽章，請參閱 [`ftxui/dom` API 參考手冊](/api/dom)。
