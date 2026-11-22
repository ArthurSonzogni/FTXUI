# Sistemas de compilación

Integra FTXUI directamente en **CMake**, **Bazel**, **Meson**, o compílala e instálala manualmente desde el código fuente.

## CMake

::: code-group

```cmake [FetchContent (Recomendado)]
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

```cmake [Submódulo Git]
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

### Opciones de CMake

| Opción | Descripción | Valor por defecto |
| --- | --- | --- |
| `FTXUI_BUILD_EXAMPLES` | Compilar los ejemplos incluidos | `OFF` |
| `FTXUI_BUILD_DOCS` | Compilar la documentación | `OFF` |
| `FTXUI_BUILD_TESTS` | Activar las pruebas unitarias | `OFF` |
| `FTXUI_ENABLE_INSTALL` | Generar los objetivos de instalación | `ON` |

Plantilla inicial: [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter).

## Bazel (Bzlmod)

FTXUI está registrada en el [Bazel Central Registry](https://registry.bazel.build/modules/ftxui).

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

Plantilla inicial: [ftxui-bazel](https://github.com/ArthurSonzogni/ftxui-bazel).

## Meson

FTXUI soporta [Meson](https://mesonbuild.com/) (≥ 1.1.0) tanto de forma independiente como en subproyecto.

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

```bash [Compilación independiente]
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

:::

## Compilación manual desde el código fuente

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

Después usa `find_package(ftxui REQUIRED)` en el `CMakeLists.txt` de tu proyecto.
