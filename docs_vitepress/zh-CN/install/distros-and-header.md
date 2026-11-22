# Linux 发行版与单头文件

在 Linux 上安装预编译的系统软件包，或者直接将合并后的单头文件/单源文件版本放进任意 C++17 项目中使用。

## Linux 发行版软件包

::: info 社区维护包
各发行版仓库中的软件包由对应 Linux 社区维护，版本可能会滞后于最新的 GitHub Release。
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

安装完成后，即可在 CMake 中直接链接：

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

## 合并版 (单头文件 / 单源文件)

如果你不想引入构建系统或包管理器，FTXUI 提供了合并（Amalgamated）发布版本：

- **分离式（编译更快）**：`ftxui.hpp` + `ftxui.cpp`
- **纯单头文件**：`ftxui_all.hpp`

### 获取方式

1. **GitHub Releases（推荐）**：从[最新发布页](https://github.com/ArthurSonzogni/FTXUI/releases)下载 `ftxui-amalgamated.zip`。
2. **本地生成**：克隆仓库后运行 `./tools/amalgamate.py`。

### 使用方式

::: code-group

```bash [分离式 (ftxui.hpp + ftxui.cpp)]
g++ -std=c++17 main.cpp ftxui.cpp -o my_app -lpthread
```

```cpp [纯单头文件 (ftxui_all.hpp)]
// Define FTXUI_IMPLEMENTATION in exactly ONE .cpp file:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

:::
