# Build Systems

Integrate FTXUI directly into **CMake**, **Bazel**, **Meson**, or build and install it manually from source.

## CMake

::: code-group

```cmake [FetchContent (Recommended)]
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

### CMake Options

| Option | Description | Default |
| --- | --- | --- |
| `FTXUI_BUILD_EXAMPLES` | Build bundled examples | `OFF` |
| `FTXUI_BUILD_DOCS` | Build the documentation | `OFF` |
| `FTXUI_BUILD_TESTS` | Enable unit tests | `OFF` |
| `FTXUI_ENABLE_INSTALL` | Generate install targets | `ON` |

Starter template: [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter).

## Bazel (Bzlmod)

FTXUI is registered in the [Bazel Central Registry](https://registry.bazel.build/modules/ftxui).

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

Starter template: [ftxui-bazel](https://github.com/ArthurSonzogni/ftxui-bazel).

## Meson

FTXUI supports [Meson](https://mesonbuild.com/) (≥ 1.1.0) both standalone and as a subproject.

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

```bash [Standalone Build]
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

:::

## Manual Build from Source

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

Then use `find_package(ftxui REQUIRED)` in your project's `CMakeLists.txt`.
