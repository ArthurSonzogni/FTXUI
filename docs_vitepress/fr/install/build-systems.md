# Systèmes de build

Intégrez directement FTXUI avec **CMake**, **Bazel**, **Meson**, ou compilez-le et installez-le manuellement depuis les sources.

## CMake

::: code-group

```cmake [FetchContent (Recommandé)]
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3 # Remplacez par une version, un tag ou un hash de commit
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

```cmake [Sous-module Git]
# Exécutez d'abord :
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

### Options CMake

| Option | Description | Valeur par défaut |
| --- | --- | --- |
| `FTXUI_BUILD_EXAMPLES` | Compiler les exemples fournis | `OFF` |
| `FTXUI_BUILD_DOCS` | Compiler la documentation | `OFF` |
| `FTXUI_BUILD_TESTS` | Activer les tests unitaires | `OFF` |
| `FTXUI_ENABLE_INSTALL` | Générer les cibles d'installation | `ON` |

Modèle de démarrage : [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter).

## Bazel (Bzlmod)

FTXUI est référencé dans le [Bazel Central Registry](https://registry.bazel.build/modules/ftxui).

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

Modèle de démarrage : [ftxui-bazel](https://github.com/ArthurSonzogni/ftxui-bazel).

## Meson

FTXUI prend en charge [Meson](https://mesonbuild.com/) (≥ 1.1.0) aussi bien en autonomie que comme sous-projet.

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

```bash [Compilation autonome]
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

:::

## Compilation manuelle depuis les sources

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

Utilisez ensuite `find_package(ftxui REQUIRED)` dans le `CMakeLists.txt` de votre projet.
