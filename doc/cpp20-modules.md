@page cpp20-modules C++20 Modules


> [!WARNING]
> This feature is still in development, and the API may change in future releases.
> Your contribution is needed to help us improve the compatibility and usability
> of C++ modules in FTXUI. If you encounter any issues or have suggestions,
> please open an issue.

FTXUI experimentally supports
[C++20 modules](https://en.cppreference.com/w/cpp/language/modules) to reduce
compilation times and improve code organization. Each part of the library has a
corresponding module, split into partitions per each header.

Use the `FTXUI_BUILD_MODULES` option to build the FTXUI project itself to provide C++20 modules,
for example with CMake and Ninja:

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```

> [!NOTE]
> To use modules, you need a C++≥20 compatible compiler, CMake version 3.20 or
> higher, and use a compatible generator like Ninja. Note that Makefile
> generators **do not support modules**.

Then, in your own code you can consume the modules and code as normal:

```cpp
import ftxui;

using ftxui::App;
using ftxui::Button;
using ftxui::Component;

int main() {
  App app = App::TerminalOutput();
  Component button = Button("Click me", app.ExitLoopClosure());
  screen.Loop(button);
  return 0;
}
```

Writing `import ftxui;` is equivalent to including all `<ftxui/**/*.hpp>` headers, and provides
the entire library through the singular module.

To properly find and link the modules with CMake, use `target_link_libraries` to get the right
compiler, linker, etc. flags.

```cmake
target_link_libraries(my_executable
    #...whatever...
    PRIVATE ftxui::modules
)
```

### Module list

While `import ftxui;` provides the entire library, FTXUI is designed in layers. If you only need specific functionalities, you can import the independent modules directly:

- `ftxui` (Convenience module that re-exports all of the below)
    - `ftxui.component` (Interactive components, events, and event loops)
    - `ftxui.dom` (Layout and styling via Elements)
    - `ftxui.screen` (Terminal rendering, pixels, and colors)
    - `ftxui.util` (Internal utilities)

For example:
```cpp
import ftxui.screen;
import ftxui.dom;

// Use only screen and dom functionalities...
```
