# Gestionnaires de paquets

Installez FTXUI via **vcpkg**, **Conan**, **xmake** ou **Nix**.

## vcpkg

FTXUI est disponible dans le [registre vcpkg](https://vcpkg.link/ports/ftxui).

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

```bash [Compilation]
vcpkg install --triplet x64-linux # ou x64-windows / arm64-osx
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

:::

## Conan

FTXUI est publié sur [Conan Center](https://conan.io/center/recipes/ftxui) à chaque nouvelle version.

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

```bash [Installation et compilation]
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

:::

::: tip Compilation Conan locale ou hors ligne
Si Conan Center est inaccessible, compilez depuis un clone local à l'aide du `conanfile.py` à la racine :
```bash
git clone https://github.com/ArthurSonzogni/FTXUI && cd FTXUI
git checkout v7.0.3
conan create . --build=missing
```
:::

## xmake

::: info Paquet communautaire
FTXUI est maintenu par la communauté dans [xmake-repo](https://github.com/xmake-io/xmake-repo/blob/dev/packages/f/ftxui/xmake.lua).
:::

```lua
add_requires("ftxui", {system = false})

target("demo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ftxui")
```

## Nix Flake

FTXUI inclut un fichier `flake.nix` fournissant à la fois les paquets et un environnement de développement.

::: code-group

```bash [Compilation]
nix build github:ArthurSonzogni/FTXUI
```

```nix [flake.nix]
{
  inputs.ftxui.url = "github:ArthurSonzogni/FTXUI";
}
```

:::

Référencez `ftxui.packages.<system>.ftxui` dans les sorties de votre flake.
