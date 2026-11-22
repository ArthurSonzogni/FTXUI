# Linux Distros & Single Header

Install pre-built system packages on Linux, or drop the amalgamated single-header/single-source distribution directly into any C++17 project.

## Linux Distribution Packages

::: info Community Packages
Distribution packages are maintained by their respective Linux communities and may lag behind the latest GitHub release.
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

Once installed, link via CMake:

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

## Amalgamated (Single Header / Single Source)

If you prefer not to use a build system or package manager, FTXUI provides amalgamated releases:

- **Split (faster builds):** `ftxui.hpp` + `ftxui.cpp`
- **Header-only:** `ftxui_all.hpp`

### How to Get It

1. **GitHub Releases (Recommended):** Download `ftxui-amalgamated.zip` from the [latest release](https://github.com/ArthurSonzogni/FTXUI/releases).
2. **Generate Locally:** Run `./tools/amalgamate.py` from a clone of the repository.

### Usage

::: code-group

```bash [Split (ftxui.hpp + ftxui.cpp)]
g++ -std=c++17 main.cpp ftxui.cpp -o my_app -lpthread
```

```cpp [Header-Only (ftxui_all.hpp)]
// Define FTXUI_IMPLEMENTATION in exactly ONE .cpp file:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

:::
