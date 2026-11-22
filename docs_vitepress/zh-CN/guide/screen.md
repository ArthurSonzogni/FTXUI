# Screen 模块 (`ftxui/screen`)

**Screen** 模块是 FTXUI 的底层基石，负责管理带样式的二维终端单元格网格（`Screen`、`Surface`、`Cell`、`Color`、`Box`）以及终端能力检测。

## `Screen` 与 `Cell`

`Screen` 表示一个由 `Cell` 组成的二维网格。可以通过 `screen.CellAt(x, y)` 直接修改任意单元格（该方法自带边界检查，越界时会安全地返回一个占位单元格）：

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

每个 `Cell` 包含以下字段：
- `character`：UTF-8 字素（grapheme）字符串
- `foreground_color`、`background_color`：`Color` 类型
- 样式标志：`bold`、`dim`、`italic`、`inverted`、`underlined`、`underlined_double`、`blink`、`strikethrough`

## 打印与原地重绘

::: code-group

```cpp [直接打印]
screen.Print();
```

```cpp [转为字符串]
std::cout << screen.ToString();
```

:::

如果要在循环中原地重绘而不让终端不断向上滚动，可以在下一帧输出前打印 `screen.ResetPosition()`：

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

`Dimension` 用于控制 `Screen::Create(width, height)` 的尺寸策略：

- `Dimension::Full()`：匹配当前终端的宽度或高度
- `Dimension::Fit(element)`：自适应某个 `Element` 所需的最小尺寸
- `Dimension::Fixed(n)`：精确指定列数或行数

## `Color` 与终端能力

`Color` 支持 4 种色彩模式，并会根据当前终端的能力自动降级到最接近的颜色：

- **默认色**：`Color::Default`
- **16 色调色板**：`Color::Red`、`Color::BlueLight` 等（[在线演示](/examples/dom/color_gallery)）
- **256 色调色板**：`Color::Palette256`、`Color::Chartreuse1` 等（[在线演示](/examples/dom/color_info_palette256)）
- **24 位真彩色 (TrueColor)**：`Color::RGB(r, g, b)`、`Color::HSV(h, s, v)`（[在线演示](/examples/dom/color_truecolor_HSV)）

可以通过 `Terminal::ColorSupport()` 和 `Terminal::SetColorSupport(...)` 查询或覆盖终端色彩支持级别（详见 [`ftxui/screen` API 参考](/api/screen)）。
