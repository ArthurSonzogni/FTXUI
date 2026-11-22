# 安装指南

FTXUI 需要支持 **C++17** 的编译器（GCC ≥ 7、Clang ≥ 5、MSVC ≥ 2017），并且**没有任何外部依赖**。

## 选择集成方式

| 分类 | 支持方式 | 文档页面 |
| --- | --- | --- |
| **构建系统** | CMake（`FetchContent`、`find_package`、子模块）、Bazel (Bzlmod)、Meson、手动源码构建 | [构建系统](/zh-CN/install/build-systems) |
| **包管理器** | vcpkg、Conan、xmake、Nix Flake | [包管理器](/zh-CN/install/package-managers) |
| **Linux 发行版与单头文件** | Debian/Ubuntu、Arch Linux、openSUSE、合并发布版（`ftxui.hpp` / `ftxui_all.hpp`） | [发行版与单头文件](/zh-CN/install/distros-and-header) |

## 最快上手方式 (CMake `FetchContent`)

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

## 库目标 (Targets)

FTXUI 拆分为三个分层目标（`component → dom → screen`），并额外提供一个聚合目标（`ftxui::ftxui`）：

| 目标 | 用途 |
| --- | --- |
| `ftxui::screen` | 二维 `Screen`/`Surface` 缓冲区、`Cell`、`Color`、`Box`、终端 I/O |
| `ftxui::dom` | 声明式 `Element` 布局树（`hbox`、`vbox`、`border`、`table`、`canvas` 等） |
| `ftxui::component` | 交互式控件（`Button`、`Input`、`Menu`、`Container`、`App` 等） |
| `ftxui::ftxui` | 聚合目标及 `<ftxui/ftxui.hpp>` 头文件，一次性引入全部三个模块 |

## 下一步

- [快速开始](/zh-CN/guide/getting-started)：构建一个小型交互式应用
- [心智模型](/zh-CN/guide/mental-model)：理解 Element、Component 与事件循环如何协作
- [在线示例](/examples/)：在浏览器中体验 WebAssembly 交互演示
