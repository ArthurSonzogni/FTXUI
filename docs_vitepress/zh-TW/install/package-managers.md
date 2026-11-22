# 套件管理員

透過 **vcpkg**、**Conan**、**xmake** 或 **Nix** 安裝 FTXUI。

## vcpkg

FTXUI 已收錄於 [vcpkg registry](https://vcpkg.link/ports/ftxui)。

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

```bash [建置]
vcpkg install --triplet x64-linux # or x64-windows / arm64-osx
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

:::

## Conan

FTXUI 每次發布新版本都會同步上架至 [Conan Center](https://conan.io/center/recipes/ftxui)。

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

```bash [安裝與建置]
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

:::

::: tip 離線／本機 Conan 建置
如果無法連線至 Conan Center，可以在本機複製儲存庫並使用根目錄的 `conanfile.py` 進行建置：
```bash
git clone https://github.com/ArthurSonzogni/FTXUI && cd FTXUI
git checkout v7.0.3
conan create . --build=missing
```
:::

## xmake

::: info 社群維護套件
FTXUI 在 [xmake-repo](https://github.com/xmake-io/xmake-repo/blob/dev/packages/f/ftxui/xmake.lua) 中由社群負責維護。
:::

```lua
add_requires("ftxui", {system = false})

target("demo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ftxui")
```

## Nix Flake

FTXUI 內附 `flake.nix`，同時提供套件建置與開發環境 shell。

::: code-group

```bash [建置]
nix build github:ArthurSonzogni/FTXUI
```

```nix [flake.nix]
{
  inputs.ftxui.url = "github:ArthurSonzogni/FTXUI";
}
```

:::

在你的 flake 輸出中直接引用 `ftxui.packages.<system>.ftxui` 即可。
