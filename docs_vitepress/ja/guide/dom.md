# DOM モジュール (`ftxui/dom`)

**DOM** モジュールは、ターミナルサイズに応じてレスポンシブにレイアウトされる、宣言的でステートレスな `Element` ノードのツリーを定義します。

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

デコレータは以下の3通りの等価な記法で適用できます。
- 関数呼び出し: `document = border(document);`
- パイプ演算子: `document = document | border;`
- 複合代入パイプ: `document |= border;`

## テキストと段落

- `text(str)`: 単一行の UTF-8 テキスト（全角文字、CJK、絵文字に対応）。
- `vtext(str)`: 縦書きテキスト（1行に1書記素）。
- `paragraph(str)`: 自動折り返しを行う複数行テキスト。`paragraphAlignLeft`、`paragraphAlignCenter`、`paragraphAlignRight`、`paragraphAlignJustify` も利用可能です。

```cpp
auto body = paragraph("Long text wraps automatically to fit the container width.")
            | borderRounded;
```

ライブサンプルの [`dom/paragraph`](/examples/dom/paragraph) と [`dom/html_like`](/examples/dom/html_like) も参照してください。

## レイアウトコンテナ

| 関数 | レイアウト |
| --- | --- |
| `hbox({...})` | 水平方向の並び |
| `vbox({...})` | 垂直方向の並び |
| `dbox({...})` | 重ね合わせレイヤー（深度ボックス。`clear_under` と併用） |
| `flexbox({...}, config)` | CSS 風の折り返しレイアウト（省略形として `hflow` / `vflow`） |
| `gridbox({ {...}, {...} })` | 2次元グリッド配置 |
| `filler()` | 伸縮する空白スペース |

利用可能なスペースの配分を制御するには、`flex`（または `flex_grow`、`flex_shrink`、`xflex`、`yflex`、`size(WIDTH, EQUAL, n)`）を使用します。

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

レイアウトのライブデモ: [`dom/vbox_hbox`](/examples/dom/vbox_hbox)、[`dom/gridbox`](/examples/dom/gridbox)、[`dom/hflow`](/examples/dom/hflow)、[`dom/dbox`](/examples/dom/dbox)、[`dom/size`](/examples/dom/size)。

## 枠線、ウィンドウ、区切り線

- **枠線:** `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(BorderStyle, Color)`.
- **`window(title, content)`:** 上辺にタイトルを埋め込んだ枠線ボックス。
- **区切り線:** `separator()`, `separatorLight()`, `separatorDouble()`, `separatorHeavy()`, `separatorDashed()`, `separatorEmpty()`, `separatorCharacter(str)`.

```cpp
auto card = window(text(" Status ") | bold,
                   vbox({
                       text("CPU: 12%"),
                       separator(),
                       text("RAM: 4.2 GB"),
                   }));
```

[`dom/border_style`](/examples/dom/border_style)、[`dom/border_colored`](/examples/dom/border_colored)、[`dom/separator_style`](/examples/dom/separator_style) も参照してください。

## 色、グラデーション、テキストスタイル

FTXUI は 16色（`Color::Red`, `Color::BlueLight`, …）、256色（`Color::Palette256`）、24ビット TrueColor（`Color::RGB(r, g, b)`, `Color::HSV(h, s, v)`）に対応しており、色数の限られたターミナルでは自動的にダウングレードします。

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

テキストスタイルのデコレータ: `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough`, `hyperlink(url)`.

[`dom/style_gallery`](/examples/dom/style_gallery)、[`dom/linear_gradient`](/examples/dom/linear_gradient)、[`dom/color_info_palette256`](/examples/dom/color_info_palette256) も参照してください。

## ゲージ、スピナー、グラフ

- `gauge(progress)`（`0.0f` から `1.0f`）、さらに `gaugeLeft`、`gaugeRight`、`gaugeUp`、`gaugeDown`。
- `spinner(charset_index, frame)`: 23種類の組み込みアニメーションスピナー。
- `graph(fn)`: `std::vector<int>(int width, int height)` コールバックを点字（Braille）スパークラインとして描画。

[`dom/gauge`](/examples/dom/gauge)、[`dom/gauge_direction`](/examples/dom/gauge_direction)、[`dom/spinner`](/examples/dom/spinner)、[`dom/graph`](/examples/dom/graph) を参照してください。

## テーブル

`<ftxui/dom/table.hpp>` をインクルードすると、整列された表を構築でき、`TableSelection` を使って行・列・矩形範囲にスタイルを適用できます。

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

すべてのスタイリング例は [`dom/table`](/examples/dom/table) を参照してください。

## Canvas

`<ftxui/dom/canvas.hpp>` をインクルードすると、サブセル単位のピクセル / 点字 / ブロック描画（線、円、テキスト、点）が行えます。

```cpp
#include <ftxui/dom/canvas.hpp>

auto c = Canvas(80, 40);
c.DrawPointCircle(40, 20, 15, Color::Cyan);
c.DrawPointLine(0, 0, 80, 40, Color::Yellow);
c.DrawText(28, 20, "FTXUI", Color::White);

auto doc = canvas(std::move(c)) | border;
```

[`dom/canvas`](/examples/dom/canvas) および [`component/canvas_animated`](/examples/component/canvas_animated) を参照してください。

## スクロール、リフレクション、レイヤー

- `frame`, `xframe`, `yframe`: `focus` を持つ子要素が見えるようにスクロールされたビューポートで要素をクリップします。`vscroll_indicator` / `hscroll_indicator` と併用します。
- `reflect(box)`: 要素の最終的な画面座標を `Box` に記録します（マウスのヒットテストに便利です）。
- `clear_under`: `dbox` 内のオーバーレイの背面セルをクリアします。
- `automerge`: 隣接する罫線文字（`─`, `│`, `┌`, `┼`）を自動的に結合します。

DOM 関数とシグネチャの一覧は [`ftxui/dom` API リファレンス](/api/dom) を参照してください。
