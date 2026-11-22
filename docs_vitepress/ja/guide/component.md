# Component モジュール (`ftxui/component`)

**Component** モジュールは、`ftxui/dom` と `ftxui/screen` の上にインタラクティブで状態を持つターミナルアプリケーションを構築するためのモジュールです。

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
```

<FtxuiDemo file="component/homescreen" height="520px" title="Live demo: component/homescreen (click tabs or use arrow keys)" />

## 組み込みインタラクティブウィジェット

すべての組み込みコンポーネントは、ポインタまたは `Ref<T>` を介して通常の C++ の状態変数へ直接バインドされます。

| コンポーネント | 簡単な例 | ライブデモ |
| --- | --- | --- |
| `Button` | `auto btn = Button("Click", [&] { count++; });` | [`component/button`](/examples/component/button) |
| `Input` | `auto in = Input(&text_val, "placeholder");` | [`component/input`](/examples/component/input) |
| `Checkbox` | `auto cb = Checkbox("Enable", &enabled);` | [`component/checkbox`](/examples/component/checkbox) |
| `Menu` | `auto m = Menu(&entries, &selected);` | [`component/menu`](/examples/component/menu) |
| `Toggle` | `auto tg = Toggle(&entries, &selected);` | [`component/toggle`](/examples/component/toggle) |
| `Radiobox` | `auto rb = Radiobox(&entries, &selected);` | [`component/radiobox`](/examples/component/radiobox) |
| `Dropdown` | `auto dd = Dropdown(&entries, &selected);` | [`component/dropdown`](/examples/component/dropdown) |
| `Slider` | `auto sl = Slider("Volume:", &val, 0, 100, 5);` | [`component/slider`](/examples/component/slider) |
| `Collapsible` | `auto col = Collapsible("Details", child);` | [`component/collapsible`](/examples/component/collapsible) |
| `Window` | `auto win = Window({.inner = child, .title = "Title"});` | [`component/window`](/examples/component/window) |
| `ResizableSplit` | `ResizableSplitLeft(left, right, &split_size)` | [`component/resizable_split`](/examples/component/resizable_split) |

各ウィジェットは `*Option` 構造体（例: `ButtonOption::Animated()`、`MenuOption::HorizontalAnimated()`、`InputOption`）も受け取り、スタイルやアニメーション、コールバック（`on_change`, `on_enter`, `transform`）をカスタマイズできます。

## レイアウトとイベントの構成 (`Container` + `Renderer`)

FTXUI では、**Container** がキーボード・マウスのルーティングとフォーカスを管理し、**`Renderer`** が視覚的なレイアウトを制御します。

```cpp
std::string username;
std::string password;

auto input_user = Input(&username, "username");
InputOption password_option;
password_option.password = true;
auto input_pass = Input(&password, "password", password_option);
auto submit = Button("Sign in", [&] { app.Exit(); });

auto form = Container::Vertical({
    input_user,
    input_pass,
    submit,
});

auto ui = Renderer(form, [&] {
  auto header = text("Login") | bold | hcenter;
  auto user_row = hbox({text("User: "), input_user->Render()});
  auto pass_row = hbox({text("Pass: "), input_pass->Render()});
  auto btn_row = submit->Render() | hcenter;

  return vbox({
             header,
             separator(),
             user_row,
             pass_row,
             separator(),
             btn_row,
         })
         | borderRounded;
});
```

### Container の種類

- `Container::Vertical({...})`: 上下矢印キーと Tab でフォーカスを縦方向に移動します。
- `Container::Horizontal({...})`: 左右矢印キーと Tab でフォーカスを横方向に移動します。
- `Container::Tab({...}, &active_tab)`: アクティブな子だけが描画され、イベントを受け取ります。
- `Container::Stacked({...})`: すべての子を順番に描画します（フローティングな `Window` ペインなどに便利です）。

## コンポーネントデコレータ

コンポーネントも DOM 要素と同じ `|` 演算子で合成できます。

- `CatchEvent(fn)`: 子に届く前にイベントを横取りします（消費する場合は `true`、伝播させる場合は `false` を返します）。詳しくは[イベント、フォーカス、マウス](/ja/guide/events-and-focus)を参照してください。
- `Maybe(&show)`: 条件に応じてコンポーネントの表示・非表示とイベント処理の有効・無効を切り替えます。
- `Modal(dialog, &show)`: `show` が `true` の間、メイン UI の上に `dialog` を重ねて背景へのイベントを遮断します。[`component/modal_dialog`](/examples/component/modal_dialog) を参照してください。
- `Hoverable(&is_hovered)`: マウスカーソルがコンポーネント上にあるかを追跡します。

```cpp
bool show_help = false;

auto ui = main_component
          | Modal(help_dialog, &show_help)
          | CatchEvent([&](Event event) {
              if (event == Event::Character('?')) {
                show_help = !show_help;
                return true;
              }
              return false;
            });
```

## バックグラウンドスレッドからの更新

`App` はイベントが到着したとき、またはフレームがリクエストされたときにのみ再描画を行います。ワーカースレッドから安全に UI の状態を更新するには、`app.Post(...)`（メインループスレッド上で実行される）を呼び出した後、`app.RequestAnimationFrame()` を呼び出します。

```cpp
std::thread worker([&] {
  auto result = fetch_data();
  app.Post([&, result = std::move(result)] {
    data = result;
  });
  app.RequestAnimationFrame();
});
```

## インラインアプリの上部への出力 (`PrintAbove`)

`App::TerminalOutput()` モードでは、インタラクティブな UI がシェルの直前の出力の下にインライン描画されます。スレッドセーフな `app.PrintAbove(element)` を使うと、ライブプロンプトの上部に永続的なログ行やチャット行を追記できます。

```cpp
auto app = App::TerminalOutput();
app.PrintAbove(paragraph("Build finished!") | color(Color::Green));
```

[`component/print_above`](/examples/component/print_above) を参照してください。

## カスタムイベントループ (`Loop`)

既存のゲームループやネットワークリアクターに FTXUI を組み込みたい場合は、`app.Loop(...)` の代わりに [`ftxui::Loop`](/api/class/loop) を使用します。

```cpp
#include <ftxui/component/loop.hpp>

auto app = App::Fullscreen();
Loop loop(&app, ui);

while (!loop.HasQuitted()) {
  update_simulation();
  app.RequestAnimationFrame();
  loop.RunOnce();
}
```

[`component/custom_loop`](/examples/component/custom_loop) を参照してください。

## ターミナル I/O の一時復元 (`WithRestoredIO`)

端末の raw モードを一時停止して外部プログラム（`$EDITOR`、`less`、`git` など）を起動するには、`app.WithRestoredIO(...)` でラップします。

```cpp
auto edit_btn = Button("Open Editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
```

[`component/with_restored_io`](/examples/component/with_restored_io) を参照してください。

## 次のステップ

- [イベント、フォーカス、マウス](/ja/guide/events-and-focus): イベントルーティング、フォーカス、マウスのヒットテスト
- [カスタムウィジェット](/ja/guide/custom-widgets): `ComponentBase` と `Node` の継承
- [`ftxui/component` API リファレンス](/api/component): 関数とオプションの全一覧
