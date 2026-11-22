# 包管理器

通过 **vcpkg**、**Conan**、**xmake** 或 **Nix** 安装 FTXUI。

## vcpkg

FTXUI 已收录于 [vcpkg registry](https://vcpkg.link/ports/ftxui)。

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

```bash [构建]
vcpkg install --triplet x64-linux # or x64-windows / arm64-osx
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

:::

## Conan

FTXUI 的每个发行版都会同步发布到 [Conan Center](https://conan.io/center/recipes/ftxui)。

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

```bash [安装与构建]
conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

:::

::: tip 离线 / 本地 Conan 构建
如果无法访问 Conan Center，可以使用仓库根目录下的 `conanfile.py` 从本地源码构建：
```bash
git clone https://github.com/ArthurSonzogni/FTXUI && cd FTXUI
git checkout v7.0.3
conan create . --build=missing
```
:::

## xmake

::: info 社区维护包
FTXUI 在 [xmake-repo](https://github.com/xmake-io/xmake-repo/blob/dev/packages/f/ftxui/xmake.lua) 中由社区维护。
:::

```lua
add_requires("ftxui", {system = false})

target("demo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ftxui")
```

## Nix Flake

FTXUI 仓库自带 `flake.nix`，同时提供软件包和开发环境 shell。

::: code-group

```bash [构建]
nix build github:ArthurSonzogni/FTXUI
```

```nix [flake.nix]
{
  inputs.ftxui.url = "github:ArthurSonzogni/FTXUI";
}
```

:::

在你的 flake 输出中引用 `ftxui.packages.<system>.ftxui` 即可。
