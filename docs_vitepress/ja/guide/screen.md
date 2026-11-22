# Screen モジュール (`ftxui/screen`)

**Screen** モジュールは FTXUI の低レベル基盤です。スタイル付きターミナルセルの 2D グリッド（`Screen`、`Surface`、`Cell`、`Color`、`Box`）と、ターミナル機能の検出を管理します。

## `Screen` と `Cell`

`Screen` は `Cell` の 2D グリッドを表します。`screen.CellAt(x, y)` を使うとセルを直接書き換えることができます（境界チェックが行われ、範囲外の場合は安全にダミーセルを返します）。

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

各 `Cell` は以下の情報を保持します。
- `character`: UTF-8 の書記素文字列
- `foreground_color`, `background_color`: `Color`
- スタイルフラグ: `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlined_double`, `blink`, `strikethrough`

## 出力と再描画

::: code-group

```cpp [直接出力]
screen.Print();
```

```cpp [文字列へ変換]
std::cout << screen.ToString();
```

:::

ループ内でターミナルをスクロールさせずに同じ位置へ再描画するには、次のフレームの前に `screen.ResetPosition()` を出力します。

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

`Dimension` は `Screen::Create(width, height)` のサイズ指定を制御します。

- `Dimension::Full()`: 現在のターミナルの幅または高さに合わせる
- `Dimension::Fit(element)`: 指定した `Element` が必要とする最小サイズに合わせる
- `Dimension::Fixed(n)`: 固定の列数または行数

## `Color` とターミナル機能

`Color` は4つのモードに対応しており、ターミナルがサポートする最も近い色へ自動的にフォールバックします。

- **デフォルト:** `Color::Default`
- **16色パレット:** `Color::Red`, `Color::BlueLight`, …（[ライブデモ](/examples/dom/color_gallery)）
- **256色パレット:** `Color::Palette256`, `Color::Chartreuse1`, …（[ライブデモ](/examples/dom/color_info_palette256)）
- **24ビット TrueColor:** `Color::RGB(r, g, b)`, `Color::HSV(h, s, v)`（[ライブデモ](/examples/dom/color_truecolor_HSV)）

色サポートの取得や上書きには `Terminal::ColorSupport()` と `Terminal::SetColorSupport(...)` を使用します（[`ftxui/screen` API リファレンス](/api/screen)を参照）。
