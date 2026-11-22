# 构建系统

直接将 FTXUI 集成到 **CMake**、**Bazel**、**Meson** 项目中，或从源码手动编译并安装。

## CMake

::: code-group

```cmake [FetchContent (推荐)]
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

```cmake [Git 子模块]
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

### CMake 选项

| 选项 | 说明 | 默认值 |
| --- | --- | --- |
| `FTXUI_BUILD_EXAMPLES` | 构建内置示例程序 | `OFF` |
| `FTXUI_BUILD_DOCS` | 构建文档 | `OFF` |
| `FTXUI_BUILD_TESTS` | 启用单元测试 | `OFF` |
| `FTXUI_ENABLE_INSTALL` | 生成安装目标 | `ON` |

项目模板：[ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter)。

## Bazel (Bzlmod)

FTXUI 已收录于 [Bazel Central Registry](https://registry.bazel.build/modules/ftxui)。

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

项目模板：[ftxui-bazel](https://github.com/ArthurSonzogni/ftxui-bazel)。

## Meson

FTXUI 支持 [Meson](https://mesonbuild.com/)（≥ 1.1.0），既可以独立构建，也可以作为子项目（subproject）引入。

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

```bash [独立构建]
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

:::

## 从源码手动构建

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

安装完成后，即可在项目的 `CMakeLists.txt` 中通过 `find_package(ftxui REQUIRED)` 引入。
