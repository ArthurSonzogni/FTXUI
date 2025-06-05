@page installation_vcpkg Vcpkg
@tableofcontents

# Vcpkg Package

FTXUI is available in the [Vcpkg registry](https://vcpkg.link/ports/ftxui)

To use it, you can add the following to your `vcpkg.json`:

```json
{
  "name": "your-project",
  "version-string": "0.1.0",
  "dependencies": [
    {
        "name": "ftxui",
        "version>=": "6.1.9"
    }
  ]
}
```

# Install FTXUI using Vcpkg
```bash
vcpkg install --triplet x64-linux  # or x64-windows / arm64-osx etc.
```

# Configure your build system.
If you are using CMake, you can use the following in your `CMakeLists.txt`:

**CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.15)
project(my_project)

# Make sure vcpkg toolchain file is passed at configure time
find_package(ftxui CONFIG REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)
```

**main.cpp**
```cpp
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

int main() {
  using namespace ftxui;

  auto screen = ScreenInteractive::TerminalOutput();
  auto button = Button("Click me", [] { std::cout << "Clicked!\n"; });

  screen.Loop(button);
}
```

**Configure and build the project**
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
./build/main
```




<div class="section_buttons">
 
| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |
 
</div>
