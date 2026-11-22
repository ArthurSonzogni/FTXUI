# Instalación

FTXUI requiere un compilador **C++17** (GCC ≥ 7, Clang ≥ 5, MSVC ≥ 2017) y tiene **cero dependencias externas**.

## Elige tu configuración

| Categoría | Opciones | Página |
| --- | --- | --- |
| **Sistemas de compilación** | CMake (`FetchContent`, `find_package`, submódulo), Bazel (Bzlmod), Meson, compilación manual | [Sistemas de compilación](/es/install/build-systems) |
| **Gestores de paquetes** | vcpkg, Conan, xmake, Nix Flake | [Gestores de paquetes](/es/install/package-managers) |
| **Distribuciones Linux y cabecera única** | Debian/Ubuntu, Arch Linux, openSUSE, amalgamado (`ftxui.hpp` / `ftxui_all.hpp`) | [Distribuciones y cabecera única](/es/install/distros-and-header) |

## Inicio más rápido (CMake `FetchContent`)

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

## Objetivos de la biblioteca

FTXUI se divide en tres objetivos en capas (`component → dom → screen`), además de un objetivo paraguas (`ftxui::ftxui`):

| Objetivo | Propósito |
| --- | --- |
| `ftxui::screen` | Búfer 2D `Screen`/`Surface`, `Cell`, `Color`, `Box`, E/S de terminal |
| `ftxui::dom` | Árbol declarativo de diseño `Element` (`hbox`, `vbox`, `border`, `table`, `canvas`, …) |
| `ftxui::component` | Widgets interactivos (`Button`, `Input`, `Menu`, `Container`, `App`, …) |
| `ftxui::ftxui` | Objetivo paraguas y cabecera `<ftxui/ftxui.hpp>` que incluye los tres |

## Próximos pasos

- [Primeros pasos](/es/guide/getting-started): construye una pequeña aplicación interactiva
- [Modelo mental](/es/guide/mental-model): cómo encajan Elements, Components y el bucle de eventos
- [Ejemplos](/examples/): demos interactivas en WebAssembly directamente en el navegador
