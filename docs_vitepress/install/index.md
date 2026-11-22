# Installation

FTXUI requires a **C++17** compiler (GCC ≥ 7, Clang ≥ 5, MSVC ≥ 2017) and has **zero external dependencies**.

## Pick Your Setup

| Category | Options | Page |
| --- | --- | --- |
| **Build Systems** | CMake (`FetchContent`, `find_package`, submodule), Bazel (Bzlmod), Meson, Manual build | [Build Systems](/install/build-systems) |
| **Package Managers** | vcpkg, Conan, xmake, Nix Flake | [Package Managers](/install/package-managers) |
| **Linux Distros & Single Header** | Debian/Ubuntu, Arch Linux, openSUSE, Amalgamated (`ftxui.hpp` / `ftxui_all.hpp`) | [Distros & Single Header](/install/distros-and-header) |

## Quickest Start (CMake `FetchContent`)

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

## Library Targets

FTXUI is split into three layered targets (`component → dom → screen`), plus an umbrella target (`ftxui::ftxui`):

| Target | Purpose |
| --- | --- |
| `ftxui::screen` | 2D `Screen`/`Surface` buffer, `Cell`, `Color`, `Box`, terminal I/O |
| `ftxui::dom` | Declarative `Element` layout tree (`hbox`, `vbox`, `border`, `table`, `canvas`, …) |
| `ftxui::component` | Interactive widgets (`Button`, `Input`, `Menu`, `Container`, `App`, …) |
| `ftxui::ftxui` | Umbrella target & `<ftxui/ftxui.hpp>` header including all three |

## Next Steps

- [Getting Started](/guide/getting-started): build a small interactive app
- [Mental Model](/guide/mental-model): how Elements, Components, and the event loop fit together
- [Examples](/examples/): interactive WebAssembly demos in the browser
