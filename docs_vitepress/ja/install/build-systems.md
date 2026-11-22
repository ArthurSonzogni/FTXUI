# ビルドシステム

**CMake**、**Bazel**、**Meson** に FTXUI を直接統合するか、ソースから手動でビルドしてインストールします。

## CMake

::: code-group

```cmake [FetchContent (推奨)]
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

### CMake オプション

| オプション | 説明 | デフォルト |
| --- | --- | --- |
| `FTXUI_BUILD_EXAMPLES` | 同梱のサンプルをビルドする | `OFF` |
| `FTXUI_BUILD_DOCS` | ドキュメントをビルドする | `OFF` |
| `FTXUI_BUILD_TESTS` | ユニットテストを有効化する | `OFF` |
| `FTXUI_ENABLE_INSTALL` | インストールターゲットを生成する | `ON` |

スターターテンプレート: [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter)

## Bazel (Bzlmod)

FTXUI は [Bazel Central Registry](https://registry.bazel.build/modules/ftxui) に登録されています。

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

スターターテンプレート: [ftxui-bazel](https://github.com/ArthurSonzogni/ftxui-bazel)

## Meson

FTXUI は [Meson](https://mesonbuild.com/)（≥ 1.1.0）に対応しており、スタンドアロンビルドとサブプロジェクト利用の両方が可能です。

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

```bash [スタンドアロンビルド]
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

:::

## ソースからの手動ビルド

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

インストール後は、プロジェクトの `CMakeLists.txt` で `find_package(ftxui REQUIRED)` を使用します。
