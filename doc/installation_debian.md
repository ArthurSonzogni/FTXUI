@page installation_debian Debian/Ubuntu

## Debian and Ubuntu Packages (Unofficial)

Pre-built packages are provided by the distributions. Install with:

```bash
sudo apt install libftxui-dev
```

The following packages are available:
- `ftxui-doc`
- `ftxui-examples`
- `libftxui-component<version>`
- `libftxui-dev`
- `libftxui-dom<version>`
- `libftxui-screen<version>`

Once installed, you can use it in your CMake projects by adding the following to
your `CMakeLists.txt`:

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
> This is an **unofficial** package. That means it is not maintained by the FTXUI
> team, but by the community. 

<div class="section_buttons">

| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |

</div>
