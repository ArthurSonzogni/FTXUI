# Linux ディストロ & シングルヘッダ

Linux のビルド済みシステムパッケージをインストールするか、統合されたシングルヘッダ / シングルソース配布物を任意の C++17 プロジェクトへ直接組み込んで利用します。

## Linux ディストリビューションのパッケージ

::: info コミュニティパッケージ
ディストリビューションのパッケージは各 Linux コミュニティによってメンテナンスされており、GitHub の最新リリースよりバージョンが遅れている場合があります。
:::

::: code-group

```bash [Debian / Ubuntu]
sudo apt install libftxui-dev
```

```bash [Arch Linux (AUR)]
yay -S ftxui
```

```bash [openSUSE]
sudo zypper addrepo https://download.opensuse.org/repositories/devel:libraries:c_c++/openSUSE_Leap_$releasever/devel:libraries:c_c++.repo
sudo zypper install ftxui
```

:::

インストール後は CMake からリンクします。

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

## 統合配布 (シングルヘッダ / シングルソース)

ビルドシステムやパッケージマネージャを使いたくない場合のために、FTXUI は統合（amalgamated）リリースを提供しています。

- **分割版（ビルドが高速）:** `ftxui.hpp` + `ftxui.cpp`
- **ヘッダオンリー版:** `ftxui_all.hpp`

### 入手方法

1. **GitHub Releases (推奨):** [最新リリース](https://github.com/ArthurSonzogni/FTXUI/releases)から `ftxui-amalgamated.zip` をダウンロードします。
2. **ローカル生成:** リポジトリをクローンし、`./tools/amalgamate.py` を実行します。

### 使い方

::: code-group

```bash [分割版 (ftxui.hpp + ftxui.cpp)]
g++ -std=c++17 main.cpp ftxui.cpp -o my_app -lpthread
```

```cpp [ヘッダオンリー版 (ftxui_all.hpp)]
// Define FTXUI_IMPLEMENTATION in exactly ONE .cpp file:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

:::
