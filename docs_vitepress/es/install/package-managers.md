# Gestores de paquetes

Instala FTXUI mediante **vcpkg**, **Conan**, **xmake** o **Nix**.

## vcpkg

FTXUI está disponible en el [registro de vcpkg](https://vcpkg.link/ports/ftxui).

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

```bash [Compilar]
vcpkg install --triplet x64-linux # or x64-windows / arm64-osx
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

:::

## Conan

FTXUI se publica en [Conan Center](https://conan.io/center/recipes/ftxui) en cada lanzamiento.

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

```bash [Instalar y compilar]
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

:::

::: tip Compilación local o sin conexión con Conan
Si Conan Center no está accesible, compila desde una copia local usando el `conanfile.py` de la raíz:
```bash
git clone https://github.com/ArthurSonzogni/FTXUI && cd FTXUI
git checkout v7.0.3
conan create . --build=missing
```
:::

## xmake

::: info Paquete comunitario
FTXUI es mantenido por la comunidad en [xmake-repo](https://github.com/xmake-io/xmake-repo/blob/dev/packages/f/ftxui/xmake.lua).
:::

```lua
add_requires("ftxui", {system = false})

target("demo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ftxui")
```

## Nix Flake

FTXUI incluye un `flake.nix` que proporciona tanto paquetes como un entorno de desarrollo.

::: code-group

```bash [Compilar]
nix build github:ArthurSonzogni/FTXUI
```

```nix [flake.nix]
{
  inputs.ftxui.url = "github:ArthurSonzogni/FTXUI";
}
```

:::

Haz referencia a `ftxui.packages.<system>.ftxui` en las salidas de tu flake.
