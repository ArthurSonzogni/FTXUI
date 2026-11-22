# Distributions Linux et en-tête unique

Installez des paquets système précompilés sous Linux, ou déposez directement la distribution amalgamée (en-tête unique ou source unique) dans n'importe quel projet C++17.

## Paquets des distributions Linux

::: info Paquets communautaires
Les paquets des distributions sont maintenus par leurs communautés Linux respectives et peuvent avoir un léger décalage par rapport à la dernière version sur GitHub.
:::

::: code-group

```bash [Debian / Ubuntu]
sudo apt install libftxui-dev
```

```bash [Arch Linux (AUR)]
yay -S ftxui
```

```bash [openSUSE]
sudo zypper addrepo https://download.opensuse.org/repositories/devel:libraries:c_c++/openSUSE_Leap_$releasever/devel:libraries:c_c++.repo
sudo zypper install ftxui
```

:::

Une fois installé, liez la bibliothèque avec CMake :

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

## Version amalgamée (en-tête unique / source unique)

Si vous préférez ne pas utiliser de système de build ni de gestionnaire de paquets, FTXUI propose des archives amalgamées :

- **Séparée (compilation plus rapide) :** `ftxui.hpp` + `ftxui.cpp`
- **En-tête unique (header-only) :** `ftxui_all.hpp`

### Comment l'obtenir

1. **Releases GitHub (Recommandé) :** téléchargez `ftxui-amalgamated.zip` depuis la [dernière release](https://github.com/ArthurSonzogni/FTXUI/releases).
2. **Génération locale :** exécutez `./tools/amalgamate.py` depuis un clone du dépôt.

### Utilisation

::: code-group

```bash [Séparée (ftxui.hpp + ftxui.cpp)]
g++ -std=c++17 main.cpp ftxui.cpp -o my_app -lpthread
```

```cpp [En-tête unique (ftxui_all.hpp)]
// Définissez FTXUI_IMPLEMENTATION dans UN SEUL fichier .cpp :
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

:::
