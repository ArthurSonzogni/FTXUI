# DOM 模块 (`ftxui/dom`)

**DOM** 模块定义了一套声明式、无状态的 `Element` 节点树，能够根据终端尺寸自动完成响应式布局。

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

装饰器有三种完全等价的写法：
- 函数调用：`document = border(document);`
- 管道操作符：`document = document | border;`
- 原地管道：`document |= border;`

## 文本与段落

- `text(str)`：单行 UTF-8 文本（原生支持全角字符、中日韩文字与 emoji）。
- `vtext(str)`：竖排文本（每行显示一个字素）。
- `paragraph(str)`：自动按词换行的多行段落，另有 `paragraphAlignLeft`、`paragraphAlignCenter`、`paragraphAlignRight`、`paragraphAlignJustify` 等对齐变体。

```cpp
auto body = paragraph("Long text wraps automatically to fit the container width.")
            | borderRounded;
```

查看在线示例：[`dom/paragraph`](/examples/dom/paragraph) 和 [`dom/html_like`](/examples/dom/html_like)。

## 布局容器

| 函数 | 布局方式 |
| --- | --- |
| `hbox({...})` | 水平行排布 |
| `vbox({...})` | 垂直列排布 |
| `dbox({...})` | 叠层排布（深度盒子，通常配合 `clear_under` 使用） |
| `flexbox({...}, config)` | 类 CSS 自动换行弹性布局（简写：`hflow` / `vflow`） |
| `gridbox({ {...}, {...} })` | 二维对齐网格 |
| `filler()` | 可伸缩空白区域 |

使用 `flex`（以及 `flex_grow`、`flex_shrink`、`xflex`、`yflex`、`size(WIDTH, EQUAL, n)`）控制元素如何分配剩余空间：

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

探索更多在线布局演示：[`dom/vbox_hbox`](/examples/dom/vbox_hbox)、[`dom/gridbox`](/examples/dom/gridbox)、[`dom/hflow`](/examples/dom/hflow)、[`dom/dbox`](/examples/dom/dbox)、[`dom/size`](/examples/dom/size)。

## 边框、窗口与分隔线

- **边框**：`border`、`borderLight`、`borderHeavy`、`borderDouble`、`borderRounded`、`borderDashed`、`borderEmpty`、`borderStyled(BorderStyle, Color)`。
- **`window(title, content)`**：顶部边框内嵌标题的带框窗口。
- **分隔线**：`separator()`、`separatorLight()`、`separatorDouble()`、`separatorHeavy()`、`separatorDashed()`、`separatorEmpty()`、`separatorCharacter(str)`。

```cpp
auto card = window(text(" Status ") | bold,
                   vbox({
                       text("CPU: 12%"),
                       separator(),
                       text("RAM: 4.2 GB"),
                   }));
```

查看在线示例：[`dom/border_style`](/examples/dom/border_style)、[`dom/border_colored`](/examples/dom/border_colored) 和 [`dom/separator_style`](/examples/dom/separator_style)。

## 颜色、渐变与文本样式

FTXUI 支持 16 色（`Color::Red`、`Color::BlueLight` 等）、256 色（`Color::Palette256`）和 24 位真彩色（`Color::RGB(r, g, b)`、`Color::HSV(h, s, v)`），并在色彩能力受限的终端上自动降级。

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

文本样式装饰器：`bold`、`dim`、`italic`、`inverted`、`underlined`、`underlinedDouble`、`blink`、`strikethrough`、`hyperlink(url)`。

查看在线示例：[`dom/style_gallery`](/examples/dom/style_gallery)、[`dom/linear_gradient`](/examples/dom/linear_gradient) 和 [`dom/color_info_palette256`](/examples/dom/color_info_palette256)。

## 进度条、加载动画与折线图

- `gauge(progress)`（取值范围 `0.0f` 到 `1.0f`），另有 `gaugeLeft`、`gaugeRight`、`gaugeUp`、`gaugeDown`。
- `spinner(charset_index, frame)`：内置 23 种动态加载动画样式。
- `graph(fn)`：将 `std::vector<int>(int width, int height)` 回调渲染为盲文点阵迷你折线图。

查看在线示例：[`dom/gauge`](/examples/dom/gauge)、[`dom/gauge_direction`](/examples/dom/gauge_direction)、[`dom/spinner`](/examples/dom/spinner) 和 [`dom/graph`](/examples/dom/graph)。

## 表格 (Table)

包含 `<ftxui/dom/table.hpp>` 即可构建对齐表格，并通过 `TableSelection` 为整行、整列或矩形区域应用样式：

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

查看 [`dom/table`](/examples/dom/table) 了解完整的表格样式展示。

## 画布 (Canvas)

包含 `<ftxui/dom/canvas.hpp>` 即可进行亚单元格级别的像素/盲文/方块绘图（支持直线、圆、文字与点）：

```cpp
#include <ftxui/dom/canvas.hpp>

auto c = Canvas(80, 40);
c.DrawPointCircle(40, 20, 15, Color::Cyan);
c.DrawPointLine(0, 0, 80, 40, Color::Yellow);
c.DrawText(28, 20, "FTXUI", Color::White);

auto doc = canvas(std::move(c)) | border;
```

查看在线示例：[`dom/canvas`](/examples/dom/canvas) 和 [`component/canvas_animated`](/examples/component/canvas_animated)。

## 滚动、坐标反射与叠层

- `frame`、`xframe`、`yframe`：将元素裁剪到视口内，并自动滚动到带有 `focus` 的子元素位置。常与 `vscroll_indicator` / `hscroll_indicator` 搭配使用。
- `reflect(box)`：把元素最终的屏幕坐标记录到 `Box` 中（常用于鼠标命中检测）。
- `clear_under`：清除 `dbox` 中浮层下方原有的单元格内容。
- `automerge`：自动合并相邻的制表符边框字符（`─`、`│`、`┌`、`┼`）。

完整的 DOM 函数列表与签名请参考 [`ftxui/dom` API 参考](/api/dom)。
