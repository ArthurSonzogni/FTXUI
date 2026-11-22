# インストール

FTXUI は **C++17** 対応コンパイラ（GCC ≥ 7、Clang ≥ 5、MSVC ≥ 2017）を必要とし、**外部依存はゼロ**です。

## セットアップ方法を選ぶ

| カテゴリ | 選択肢 | ページ |
| --- | --- | --- |
| **ビルドシステム** | CMake (`FetchContent`, `find_package`, submodule), Bazel (Bzlmod), Meson, 手動ビルド | [ビルドシステム](/ja/install/build-systems) |
| **パッケージマネージャ** | vcpkg, Conan, xmake, Nix Flake | [パッケージマネージャ](/ja/install/package-managers) |
| **Linux ディストロ & シングルヘッダ** | Debian/Ubuntu, Arch Linux, openSUSE, 統合配布 (`ftxui.hpp` / `ftxui_all.hpp`) | [ディストロ & シングルヘッダ](/ja/install/distros-and-header) |

## 最速セットアップ (CMake `FetchContent`)

```cmake
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3
)
FetchContent_MakeAvailable(ftxui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ftxui::component) # transitively links dom & screen
```

## ライブラリターゲット

FTXUI は3つの階層ターゲット（`component → dom → screen`）と、それらをまとめたアンブレラターゲット（`ftxui::ftxui`）で構成されています。

| ターゲット | 役割 |
| --- | --- |
| `ftxui::screen` | 2D の `Screen`/`Surface` バッファ、`Cell`、`Color`、`Box`、ターミナル I/O |
| `ftxui::dom` | 宣言的な `Element` レイアウトツリー（`hbox`, `vbox`, `border`, `table`, `canvas`, …） |
| `ftxui::component` | インタラクティブなウィジェット（`Button`, `Input`, `Menu`, `Container`, `App`, …） |
| `ftxui::ftxui` | 上記3つをすべて含むアンブレラターゲットおよび `<ftxui/ftxui.hpp>` ヘッダ |

## 次のステップ

- [はじめに](/ja/guide/getting-started): 小さなインタラクティブアプリを作成する
- [メンタルモデル](/ja/guide/mental-model): Element、Component、イベントループの連携を理解する
- [サンプル集](/examples/): ブラウザ上で動く WebAssembly ライブデモ
