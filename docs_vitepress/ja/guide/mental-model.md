# メンタルモデル

FTXUI はいくつかの核となる設計思想に基づいています。これらを最初に把握しておくと、API をずっと直感的に使いこなせるようになります。

## 1. 3層のモジュール構成

FTXUI は互いに積み重なる3つのモジュールで構成されています（`screen → dom → component`）。

- **`ftxui/screen`**（[ガイド](/ja/guide/screen)、[API](/api/screen)）: スタイル付きセルによる低レベルの2Dグリッド（`Screen`、`Surface`、`Cell`、`Color`、`Box`）。
- **`ftxui/dom`**（[ガイド](/ja/guide/dom)、[API](/api/dom)）: ステートレスなレイアウトツリー（`Element`、`hbox`、`vbox`、`border`、`table`、`canvas`、`Render()`）。
- **`ftxui/component`**（[ガイド](/ja/guide/component)、[API](/api/component)）: インタラクティブなウィジェットとイベントループ（`Component`、`Button`、`Input`、`Menu`、`Container`、`App`）。

アンブレラターゲット（CMake では `ftxui::ftxui`、Bazel では `@ftxui//:ftxui`、ヘッダは `#include <ftxui/ftxui.hpp>`）を使えば、3つすべてをまとめて利用することもできます。

## 2. Element と Component の違い

| | **Element** (`ftxui/dom`) | **Component** (`ftxui/component`) |
| --- | --- | --- |
| **ライフタイム** | 一時的。毎フレーム再構築される | 永続的。`app.Loop(...)` の前に一度生成される |
| **状態** | ステートレスな値のツリー | フォーカス状態と、ユーザー側 C++ 状態への参照を保持 |
| **入力** | イベントを受け取らない | `OnEvent` 経由でキーボード・マウスの `Event` を処理 |
| **例** | `text`, `hbox`, `vbox`, `border`, `gauge`, `table`, `canvas` | `Button`, `Input`, `Menu`, `Slider`, `Modal`, `Window`, `Container` |

UI は C++ の状態から計算される関数です。ウィジェットのラベルを直接書き換えるのではなく、通常の C++ 変数（`std::string`、`int`、`bool`、`std::vector`）を更新し、次のフレームの `Render()` で新しい `Element` ツリーを構築します。

## 3. イベントツリーと視覚的レイアウトツリーの分離

FTXUI では、**イベント・フォーカスのルーティング**と**視覚的なレイアウト**が意図的に分離されています。

- **Container**（`Container::Vertical`、`Container::Horizontal`、`Container::Tab`、`Container::Stacked`）は**イベントとフォーカスのツリー**を定義します（どの子がキーボード入力を受け取り、矢印キーや `Tab` でフォーカスがどう移動するか）。
- **`Renderer(container, [&] { ... })`** はすべてのイベントを `container` に転送しつつ、**視覚的なレイアウト**を定義します。

::: danger よくある落とし穴
`Renderer([&] { ... })` の中で `child->Render()` を呼び出しても、第1引数にその子コンポーネント（またはすべての子を含むコンテナ）を渡していなければ、キーボードやマウスのイベントは子コンポーネントに**一切届きません**。
:::

```cpp
// WRONG: `input` and `button` are drawn, but never receive events!
auto ui = Renderer([&] {
  return vbox({input->Render(), button->Render()});
});

// RIGHT: `container` routes events & focus; `Renderer` controls layout.
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({
      text("Login") | bold | hcenter,
      separator(),
      input->Render(),
      button->Render(),
  }) | border;
});
```

ラムダの中で `container->Render()` を呼ぶ必要はありません。各子の `->Render()` を個別に呼び出すことで、`vbox`、`hbox`、`window`、`gridbox` など任意のレイアウトに配置できます。

## 4. フレームのライフサイクル

```text
  Terminal input / App::Post / RequestAnimationFrame
                     │
                     ▼
        root_component->OnEvent(event)
        (mutates your plain C++ state)
                     │
                     ▼
        Element tree = root_component->Render()
        (fresh stateless DOM tree from state)
                     │
                     ▼
        Render(screen, tree) → diff → terminal
```

- FTXUI はイベントが到着したとき、またはアニメーションフレームがリクエストされたときにのみ再描画を行います。
- 画面出力は前フレームとの**差分（diff）**を取り、ちらつきやティアリングを防ぐために同期更新（DEC mode 2026）でラップして送信されます。

## 5. `App` モードの選び方

| ファクトリ関数 | 動作 | 適した用途 |
| --- | --- | --- |
| `App::Fullscreen()` | 代替スクリーンバッファを使用（端末全体のサイズを使い、終了時に復元）。 | フルスクリーン TUI（ダッシュボード、エディタ、ファイルマネージャ） |
| `App::TerminalOutput()` | シェルの直前の出力の下にインライン描画。`PrintAbove()` に対応。 | インタラクティブ CLI、プロンプト、進捗表示やログ出力ツール |
| `App::FitComponent()` | コンポーネントの最小幅・高さに合わせて自動リサイズ。 | コンパクトな埋め込みウィジェットや簡易プロンプト |
| `App::FixedSize(w, h)` | 固定の幅と高さで描画。 | テストや固定レイアウト表示 |
