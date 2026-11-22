# C++20 モジュール

::: warning 実験的機能
この機能は現在開発中であり、今後のリリースで API が変更される可能性があります。不具合や改善提案があれば、GitHub で Issue を作成してください。
:::

FTXUI はコンパイル時間の短縮とコード構成の改善のため、[C++20 モジュール](https://en.cppreference.com/w/cpp/language/modules)を実験的にサポートしています。ライブラリの各パーツに対応するモジュールが用意されており、ヘッダごとにパーティションへ分割されています。

FTXUI プロジェクト本体を C++20 モジュール付きでビルドするには、`FTXUI_BUILD_MODULES` オプションを有効にします（以下は CMake と Ninja を使う例です）。

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```

::: info 注意
モジュールを使用するには、C++20 対応コンパイラ、CMake 3.20 以降、および Ninja などの対応ジェネレータが必要です。Makefile ジェネレータは C++20 モジュールをサポートしていません。
:::

ビルドが完了したら、自分のコードから通常どおりモジュールをインポートして利用できます。

```cpp
import ftxui;

using ftxui::App;
using ftxui::Button;
using ftxui::Component;

int main() {
  App app = App::TerminalOutput();
  Component button = Button("Click me", app.ExitLoopClosure());
  app.Loop(button);
  return 0;
}
```

`import ftxui;` はすべての `<ftxui/**/*.hpp>` ヘッダをインクルードするのと同じであり、単一のモジュールを通じてライブラリ全体を提供します。

CMake でモジュールを適切に検索・リンクし、必要なコンパイラフラグやリンカフラグを得るには `target_link_libraries` を使用します。

```cmake
target_link_libraries(my_executable
    #...whatever...
    PRIVATE ftxui::modules
)
```

### モジュール一覧

`import ftxui;` はライブラリ全体を提供しますが、FTXUI はレイヤー構造で設計されています。特定の機能だけが必要な場合は、独立したモジュールを直接インポートできます。

- `ftxui`（以下のすべてを再エクスポートする便利な集約モジュール）
    - `ftxui.component`（インタラクティブコンポーネント、イベント、イベントループ）
    - `ftxui.dom`（Element によるレイアウトとスタイリング）
    - `ftxui.screen`（ターミナル描画、セル、色）
    - `ftxui.util`（内部ユーティリティ）

例:
```cpp
import ftxui.screen;
import ftxui.dom;

// Use only screen and dom functionalities...
```
