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

Use the FTXUI_BUILD_MODULES option to build the FTXUI project itself to provide C++20 modules,
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

using ftxui::Button;
using ftxui::ScreenInteractive;

int main() {
  auto screen = ScreenInteractive::TerminalOutput();
  auto button = Button("Click me", screen.QuitClosure());
  screen.Loop(button);
  return 0;
}
```

Note, the `ftxui` convenience module which simply pulls together all the modules:

```cpp
export import ftxui.component;
export import ftxui.dom;
export import ftxui.screen;
export import ftxui.util;
```
You can instead import only the module(s) you need if desired.

To properly find and link the modules with CMake, use `target_link_libraries` to get the right
compiler, linker, etc. flags.

```cmake
target_link_libraries(my_executable
    #...whatever...
    PRIVATE ftxui::modules
)
```

### Module list

The modules directly reference the corresponding header, or a group of related
headers to provide a more convenient interface. The following modules
are available:

- `ftxui`
    - `ftxui.component`
    - `ftxui.dom`
    - `ftxui.screen`
    - `ftxui.util`
