# Package Managers

Install FTXUI via **vcpkg**, **Conan**, **xmake**, or **Nix**.

## vcpkg

FTXUI is available in the [vcpkg registry](https://vcpkg.link/ports/ftxui).

::: code-group

```json [vcpkg.json]
{
  "name": "your-project",
  "version-string": "0.1.0",
  "dependencies": [
    {
      "name": "ftxui",
      "version>=": "7.0.3"
    }
  ]
}
```

```cmake [CMakeLists.txt]
cmake_minimum_required(VERSION 3.15)
project(my_project)

find_package(ftxui CONFIG REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

```bash [Build]
vcpkg install --triplet x64-linux # or x64-windows / arm64-osx
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

:::

## Conan

FTXUI is published on [Conan Center](https://conan.io/center/recipes/ftxui) on every release.

::: code-group

```ini [conanfile.txt]
[requires]
ftxui/7.0.3

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout
```

```cmake [CMakeLists.txt]
cmake_minimum_required(VERSION 3.20)
project(ftxui-demo)
set(CMAKE_CXX_STANDARD 17)

find_package(ftxui CONFIG REQUIRED)

add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE ftxui::component)
```

```bash [Install & Build]
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

:::

::: tip Offline / Local Conan Build
If Conan Center is unreachable, build from a local checkout using the root `conanfile.py`:
```bash
git clone https://github.com/ArthurSonzogni/FTXUI && cd FTXUI
git checkout v7.0.3
conan create . --build=missing
```
:::

## xmake

::: info Community Package
FTXUI is maintained by the community in [xmake-repo](https://github.com/xmake-io/xmake-repo/blob/dev/packages/f/ftxui/xmake.lua).
:::

```lua
add_requires("ftxui", {system = false})

target("demo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ftxui")
```

## Nix Flake

FTXUI ships with a `flake.nix` providing both packages and a development shell.

::: code-group

```bash [Build]
nix build github:ArthurSonzogni/FTXUI
```

```nix [flake.nix]
{
  inputs.ftxui.url = "github:ArthurSonzogni/FTXUI";
}
```

:::

Reference `ftxui.packages.<system>.ftxui` in your flake outputs.
