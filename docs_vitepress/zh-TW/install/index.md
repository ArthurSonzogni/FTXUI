# 安裝指南

FTXUI 需要支援 **C++17** 的編譯器（GCC ≥ 7、Clang ≥ 5、MSVC ≥ 2017），且**完全沒有外部相依套件**。

## 選擇安裝方式

| 分類 | 支援項目 | 說明頁面 |
| --- | --- | --- |
| **建置系統** | CMake（`FetchContent`、`find_package`、submodule）、Bazel（Bzlmod）、Meson、手動從原始碼建置 | [建置系統](/zh-TW/install/build-systems) |
| **套件管理員** | vcpkg、Conan、xmake、Nix Flake | [套件管理員](/zh-TW/install/package-managers) |
| **Linux 發行版與單一標頭檔** | Debian/Ubuntu、Arch Linux、openSUSE、合併檔（`ftxui.hpp` / `ftxui_all.hpp`） | [Linux 發行版與單一標頭檔](/zh-TW/install/distros-and-header) |

## 最快速的開始方式（CMake `FetchContent`）

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

## 函式庫目標（Targets）

FTXUI 拆分為三個分層目標（`component → dom → screen`），外加一個整合目標（`ftxui::ftxui`）：

| 目標 | 用途 |
| --- | --- |
| `ftxui::screen` | 二維 `Screen`/`Surface` 緩衝區、`Cell`、`Color`、`Box`、終端機 I/O |
| `ftxui::dom` | 宣告式 `Element` 版面配置樹（`hbox`、`vbox`、`border`、`table`、`canvas` 等） |
| `ftxui::component` | 互動式元件（`Button`、`Input`、`Menu`、`Container`、`App` 等） |
| `ftxui::ftxui` | 包含上述三者的整合目標與 `<ftxui/ftxui.hpp>` 標頭檔 |

## 下一步

- [快速開始](/zh-TW/guide/getting-started)：建立一個小型互動式應用程式
- [心智模型](/zh-TW/guide/mental-model)：了解 Element、Component 與事件迴圈如何協同運作
- [線上範例](/examples/)：直接在瀏覽器中體驗 WebAssembly 互動範例
