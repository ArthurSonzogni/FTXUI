# イベント、フォーカス、マウス

FTXUI がキーボードイベントをルーティングし、コンポーネント間でフォーカスを移動し、マウス入力を処理する仕組みを解説します。

## イベントがツリーを伝播する流れ

キー入力、マウス操作、またはリサイズが発生すると、次のように処理されます。

1. `App` が `root->OnEvent(event)` を通じて**ルートコンポーネント**に `Event` を渡します。
2. コンテナ（`Container::Vertical`、`Container::Horizontal`、`Container::Tab`、`Container::Stacked`）は、まず**現在アクティブな子**にキーボードイベントを転送します。
3. 子が `true` を返した場合、イベントは**消費**され、伝播はそこで停止します。
4. 子が `false` を返した場合、コンテナ自身がナビゲーションキーを処理します（例: `Container::Vertical` の `ArrowUp`/`ArrowDown`、`Container::Horizontal` の `ArrowLeft`/`ArrowRight`、フォーカス可能な子間の `Tab`/`TabReverse`）。

```text
  App → root->OnEvent(e)
           │
           ▼
     active child->OnEvent(e)
       ├── returns true  → done (event consumed)
       └── returns false → container handles navigation / bubbles up
```

## `CatchEvent` によるイベントの横取り

`CatchEvent` はコンポーネントをラップし、内部のコンポーネントに届く**前**にすべての `Event` を検査します。

```cpp
auto app = App::Fullscreen();

auto ui = main_component
          | CatchEvent([&](Event event) {
              if (event == Event::Character('q')) {
                app.Exit();
                return true;  // Consumed: inner component won't see 'q'
              }
              return false;   // Not consumed: pass to main_component
            });
```

::: warning グローバルショートカットと `Input` の競合
`CatchEvent` は子コンポーネントより**先**に実行されるため、ルートで `Event::Character('q')` を捕捉すると、`Input` 欄に `'q'` を入力できなくなります。対策としては次のいずれかを行ってください。
- 修飾キーを使う（`Event::CtrlQ`、`Event::Escape` など）
- 印字可能文字を消費する前に `!input->Focused()` をチェックする
:::

### よく使う `Event` 判定

```cpp
if (event == Event::Return) { /* Enter */ }
if (event == Event::Escape) { /* Esc */ }
if (event == Event::CtrlC)  { /* Ctrl+C */ }
if (event.is_character())   { std::string ch = event.character(); }
if (event.is_mouse())       { Mouse m = event.mouse(); }
```

## フォーカスとコンテナ

- フォーカスを受け取れるのは `Focusable()` が `true` を返すコンポーネントだけです。フォーカス可能な子を持たない `Renderer(...)` はフォーカスを受け取れません。
- **`Container::Vertical` / `Container::Horizontal`** はアクティブな子のインデックスを自動的に追跡します（明示的に `int* selected` ポインタを渡してフォーカスを監視・制御することもできます）。
- `Container::Tab({tab0, tab1, tab2}, &tab_index)` は `children[tab_index]` にのみイベントをルーティングします。非アクティブなタブはイベントを受け取らず、フォーカスも持ちません。
- **プログラムからのフォーカス移動:** `child->TakeFocus()` を呼ぶと、指定したコンポーネントへ直接フォーカスを移せます。
- **フォーカス状態の取得:** `Renderer` の中で `child->Focused()`（そのリーフ自身がフォーカス中）や `child->Active()`（その子または子孫がフォーカス中）を呼び出し、アクティブなペインの見た目を切り替えることができます。

```cpp
auto pane = Renderer(child, [&] {
  auto border_style = child->Active() ? borderDouble : borderLight;
  return child->Render() | border_style;
});
```

## モーダルとイベントの遮断

`Modal(dialog, &show)` は `dialog` をメイン UI の前面に重ね（`dbox` + `clear_under` を使用）、`show == true` の間は背景へのキーボード・マウスイベントをすべて遮断します。

```cpp
bool show_modal = false;
auto ui = main_component
          | Modal(dialog_component, &show_modal);
```

`show_modal` が `false` になると、イベントは即座に `main_component` へ再びルーティングされるようになります。

## マウスイベントとヒットテスト

`App` ではマウスサポートが自動的に有効化されます。`Mouse` 構造体には以下の情報が含まれます。

- `mouse.x`, `mouse.y`: 0 始まりの端末セル座標
- `mouse.button`: `Mouse::Left`, `Mouse::Middle`, `Mouse::Right`, `Mouse::None`, `Mouse::WheelUp`, `Mouse::WheelDown`
- `mouse.motion`: `Mouse::Pressed`, `Mouse::Released`, `Mouse::Moved`
- 修飾キー: `mouse.shift`, `mouse.meta`, `mouse.control`

カスタム領域のクリックやホバーを検出するには、`Render()` 中に `reflect(box)` で描画矩形を記録し、`OnEvent()` 内で `box.Contain(x, y)` を判定します。

```cpp
Box card_box;
bool hovered = false;

auto card = Renderer([&] {
  auto style = hovered ? color(Color::Yellow) : color(Color::White);
  return text(" Click or hover me ")
         | style
         | borderRounded
         | reflect(card_box);
});

auto interactive_card = card
                        | CatchEvent([&](Event event) {
                            if (!event.is_mouse()) {
                              return false;
                            }
                            hovered = card_box.Contain(event.mouse().x, event.mouse().y);
                            if (hovered &&
                                event.mouse().button == Mouse::Left &&
                                event.mouse().motion == Mouse::Pressed) {
                              // Handle click
                              return true;
                            }
                            return false;
                          });
```

::: tip 組み込みの `Hoverable`
ホバー状態の bool 値だけが欲しい場合は、ヒットテストを手書きせず組み込みのデコレータを利用できます。
```cpp
bool is_hovered = false;
auto card = base_component | Hoverable(&is_hovered);
```
:::
