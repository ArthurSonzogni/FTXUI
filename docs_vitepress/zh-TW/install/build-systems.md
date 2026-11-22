# 建置系統

將 FTXUI 直接整合至 **CMake**、**Bazel**、**Meson**，或是從原始碼手動建置與安裝。

## CMake

::: code-group

```cmake [FetchContent（推薦）]
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3 # Replace with a version, tag, or commit hash
)
FetchContent_MakeAvailable(ftxui)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

```cmake [find_package]
find_package(ftxui REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

```cmake [Git Submodule]
# First run:
#   git submodule add https://github.com/ArthurSonzogni/FTXUI external/ftxui
#   git submodule update --init --recursive

add_subdirectory(external/ftxui)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

:::

### CMake 選項

| 選項 | 說明 | 預設值 |
| --- | --- | --- |
| `FTXUI_BUILD_EXAMPLES` | 建置內附範例程式 | `OFF` |
| `FTXUI_BUILD_DOCS` | 建置說明文件 | `OFF` |
| `FTXUI_BUILD_TESTS` | 啟用單元測試 | `OFF` |
| `FTXUI_ENABLE_INSTALL` | 產生安裝目標（install targets） | `ON` |

入門範本專案：[ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter)。

## Bazel（Bzlmod）

FTXUI 已收錄於 [Bazel Central Registry](https://registry.bazel.build/modules/ftxui)。

::: code-group

```python [MODULE.bazel]
bazel_dep(name = "ftxui", version = "7.0.3")
```

```python [BUILD.bazel]
cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "@ftxui//:component",
        "@ftxui//:dom",
        "@ftxui//:screen",
    ],
)
```

:::

入門範本專案：[ftxui-bazel](https://github.com/ArthurSonzogni/ftxui-bazel)。

## Meson

FTXUI 支援 [Meson](https://mesonbuild.com/)（≥ 1.1.0），可獨立建置或作為子專案（subproject）引入。

::: code-group

```ini [subprojects/ftxui.wrap]
[wrap-git]
url = https://github.com/ArthurSonzogni/FTXUI.git
revision = v7.0.3

[provide]
ftxui-screen = ftxui_screen_dep
ftxui-dom = ftxui_dom_dep
ftxui-component = ftxui_component_dep
```

```python [meson.build]
ftxui_component_dep = dependency('ftxui-component',
  fallback: ['ftxui', 'ftxui_component_dep'],
)

executable('my_app',
  'main.cpp',
  dependencies: ftxui_component_dep,
)
```

```bash [獨立建置]
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

:::

## 從原始碼手動建置

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

安裝完成後，即可在你的專案 `CMakeLists.txt` 中使用 `find_package(ftxui REQUIRED)`。
