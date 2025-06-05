@page installation_arch Arch Linux

FTXUI is packaged on the AUR. Install using an AUR helper:

```bash
yay -S ftxui
```

You can also manually download the PKGBUILD from <https://aur.archlinux.org/packages/ftxui>.

Once installed, you can use it in your CMake projects by adding the following to your `CMakeLists.txt`:

```cmake
find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

> [!note]
> This is an unofficial package. That means it is not maintained by the FTXUI
> team, but by the community. The package maintainers seems to actively update
> the package to the latest version. Thanks to the maintainers for their work!

<div class="section_buttons">

| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |

</div>
