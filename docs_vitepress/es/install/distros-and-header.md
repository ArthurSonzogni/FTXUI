# Distribuciones Linux y cabecera única

Instala paquetes precompilados del sistema en Linux, o añade la distribución amalgamada de cabecera única o fuente única directamente a cualquier proyecto C++17.

## Paquetes de distribuciones Linux

::: info Paquetes comunitarios
Los paquetes de distribución los mantienen sus respectivas comunidades de Linux y pueden ir por detrás de la última versión en GitHub.
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

Una vez instalado, enlázalo con CMake:

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

## Amalgamado (cabecera única / fuente única)

Si prefieres no usar un sistema de compilación ni un gestor de paquetes, FTXUI ofrece versiones amalgamadas:

- **Dividida (compilación más rápida):** `ftxui.hpp` + `ftxui.cpp`
- **Solo cabecera (header-only):** `ftxui_all.hpp`

### Cómo obtenerlo

1. **Lanzamientos de GitHub (recomendado):** descarga `ftxui-amalgamated.zip` desde el [último lanzamiento](https://github.com/ArthurSonzogni/FTXUI/releases).
2. **Generarlo localmente:** ejecuta `./tools/amalgamate.py` desde un clon del repositorio.

### Uso

::: code-group

```bash [Dividida (ftxui.hpp + ftxui.cpp)]
g++ -std=c++17 main.cpp ftxui.cpp -o my_app -lpthread
```

```cpp [Solo cabecera (ftxui_all.hpp)]
// Define FTXUI_IMPLEMENTATION in exactly ONE .cpp file:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

:::
