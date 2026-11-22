# Screen 模組（`ftxui/screen`）

**Screen** 模組是 FTXUI 的底層基礎，負責管理帶有樣式的終端機二維字元格網格（`Screen`、`Surface`、`Cell`、`Color`、`Box`）以及終端機功能偵測。

## `Screen` 與 `Cell`

`Screen` 代表一個由 `Cell` 組成的二維網格。你可以透過 `screen.CellAt(x, y)` 直接修改字元格（此方法包含邊界檢查，若座標超出範圍會安全回傳一個虛擬字元格）：

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

每個 `Cell` 會儲存：
- `character`：UTF-8 字素（grapheme）字串
- `foreground_color`、`background_color`：`Color`
- 樣式旗標：`bold`、`dim`、`italic`、`inverted`、`underlined`、`underlined_double`、`blink`、`strikethrough`

## 輸出與重繪

::: code-group

```cpp [直接輸出]
screen.Print();
```

```cpp [轉成字串]
std::cout << screen.ToString();
```

:::

若要在迴圈中原地重繪而不讓終端機捲動，請在繪製下一幀之前輸出 `screen.ResetPosition()`：

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

`Dimension` 用來控制 `Screen::Create(width, height)` 的尺寸計算方式：

- `Dimension::Full()`：符合目前終端機的寬度或高度
- `Dimension::Fit(element)`：貼合指定 `Element` 所需的最小尺寸
- `Dimension::Fixed(n)`：指定的固定欄數或列數

## `Color` 與終端機色彩支援

`Color` 支援 4 種模式，並會自動降級至終端機支援的最接近色彩：

- **預設**：`Color::Default`
- **16 色調色盤**：`Color::Red`、`Color::BlueLight` 等（[線上範例](/examples/dom/color_gallery)）
- **256 色調色盤**：`Color::Palette256`、`Color::Chartreuse1` 等（[線上範例](/examples/dom/color_info_palette256)）
- **24 位元 TrueColor**：`Color::RGB(r, g, b)`、`Color::HSV(h, s, v)`（[線上範例](/examples/dom/color_truecolor_HSV)）

你可以使用 `Terminal::ColorSupport()` 查詢終端機的色彩支援等級，或透過 `Terminal::SetColorSupport(...)` 手動覆寫（詳見 [`ftxui/screen` API 參考手冊](/api/screen)）。
