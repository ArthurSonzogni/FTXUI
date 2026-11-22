# Linux 發行版與單一標頭檔

你可以直接安裝 Linux 發行版預先編譯好的系統套件，或是將合併版（amalgamated）的單一標頭檔／單一原始碼檔直接放進任何 C++17 專案中使用。

## Linux 發行版套件

::: info 社群維護套件
各發行版套件由對應的 Linux 社群維護，版本可能會略晚於 GitHub 最新發布版本。
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

安裝完成後，透過 CMake 連結即可：

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

## 合併版（單一標頭檔／單一原始碼檔）

如果你不想使用建置系統或套件管理員，FTXUI 也提供合併版發行檔案：

- **分離式（編譯較快）**：`ftxui.hpp` + `ftxui.cpp`
- **純標頭檔（Header-only）**：`ftxui_all.hpp`

### 取得方式

1. **GitHub Releases（推薦）**：從[最新版本頁面](https://github.com/ArthurSonzogni/FTXUI/releases)下載 `ftxui-amalgamated.zip`。
2. **本機產生**：複製儲存庫後執行 `./tools/amalgamate.py`。

### 使用方式

::: code-group

```bash [分離式（ftxui.hpp + ftxui.cpp）]
g++ -std=c++17 main.cpp ftxui.cpp -o my_app -lpthread
```

```cpp [純標頭檔（ftxui_all.hpp）]
// Define FTXUI_IMPLEMENTATION in exactly ONE .cpp file:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

:::
