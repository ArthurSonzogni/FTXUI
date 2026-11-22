# Installation

FTXUI nécessite un compilateur **C++17** (GCC ≥ 7, Clang ≥ 5, MSVC ≥ 2017) et n'a **aucune dépendance externe**.

## Choisissez votre configuration

| Catégorie | Options | Page |
| --- | --- | --- |
| **Systèmes de build** | CMake (`FetchContent`, `find_package`, sous-module), Bazel (Bzlmod), Meson, compilation manuelle | [Systèmes de build](/fr/install/build-systems) |
| **Gestionnaires de paquets** | vcpkg, Conan, xmake, Nix Flake | [Gestionnaires de paquets](/fr/install/package-managers) |
| **Distributions Linux et en-tête unique** | Debian/Ubuntu, Arch Linux, openSUSE, version amalgamée (`ftxui.hpp` / `ftxui_all.hpp`) | [Distributions et en-tête unique](/fr/install/distros-and-header) |

## Démarrage le plus rapide (CMake `FetchContent`)

```cmake
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3
)
FetchContent_MakeAvailable(ftxui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ftxui::component) # lie transitivement dom et screen
```

## Cibles de la bibliothèque

FTXUI est découpé en trois cibles en couches (`component → dom → screen`), plus une cible parapluie (`ftxui::ftxui`) :

| Cible | Rôle |
| --- | --- |
| `ftxui::screen` | Tampon 2D `Screen`/`Surface`, `Cell`, `Color`, `Box`, entrées/sorties terminal |
| `ftxui::dom` | Arbre de mise en page déclaratif `Element` (`hbox`, `vbox`, `border`, `table`, `canvas`, …) |
| `ftxui::component` | Widgets interactifs (`Button`, `Input`, `Menu`, `Container`, `App`, …) |
| `ftxui::ftxui` | Cible parapluie et en-tête `<ftxui/ftxui.hpp>` incluant les trois |

## Prochaines étapes

- [Premiers pas](/fr/guide/getting-started) : construire une petite application interactive
- [Modèle mental](/fr/guide/mental-model) : comment les Elements, les Components et la boucle d'événements s'articulent
- [Exemples](/examples/) : démos interactives WebAssembly dans le navigateur
