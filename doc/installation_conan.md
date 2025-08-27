@page installation_conan Conan
@tableofcontents

FTXUI can be easily obtained and integrated into your project using the Conan package manager.

## Prerequisites

First, ensure that Conan is installed on your system. If not, you can install it via pip:

```powershell
pip install conan
```
Conan often works in tandem with CMake, so you will need to have CMake installed as well. Once you have confirmed both Conan and CMake are installed, create a project directory, for example, `ftxui-demo`:

```powershell
mkdir C:\ftxui-demo
cd C:\ftxui-demo
```

## Configuration

After ensuring your environment is set up correctly, create a Conan configuration file `conanfile.txt`. This file is used to declare your project's dependencies. The community-maintained package for FTXUI can be found on [Conan Center](https://conan.io/center/recipes/ftxui).

> [!note]
> This is an unofficial build script. This means it is not maintained by the FTXUI
> team but by the community. The package maintainer appears to actively update it
> to the latest releases. Many thanks to the maintainer for their work!

@todo If you are familiar with the process, please consider adding an "official" build script to Conan Center.
This could be a GitHub Action that automatically updates Conan Center upon new releases.

```ini
[requires]
ftxui/6.0.2

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout
```

## Install Dependencies and Build

Once configured, run the following command to install FTXUI and its dependencies:

```powershell
conan install . --output-folder=build --build=missing
```

This will download and install `ftxui/6.0.2` along with all its dependencies from Conan's remote repositories.

After the installation completes, you can test it by creating a `demo.cpp` file in your project directory:

```cpp
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

int main() {
    using namespace ftxui;
    auto document = hbox({
        text(" Hello "),
        text("FTXUI ") | bold | color(Color::Red),
        text(" world! ")
    });
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    std::cout << screen.ToString() << std::endl;
    return 0;
}
```

If the test is successful, you can then create a `CMakeLists.txt` file in the project directory:

```cmake
cmake_minimum_required(VERSION 3.20)
project(ftxui-demo)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Find the FTXUI package installed via Conan
find_package(ftxui CONFIG REQUIRED)

# Create the executable
add_executable(demo demo.cpp)

# Link the executable to the FTXUI library
target_link_libraries(demo PRIVATE ftxui::component)
```

@todo 考虑到中国多数地区使用Conan很有可能遇到各种网络问题，我想做一个定制的版本说明，但是我对conan的了解有限再加上没有找到合适的资料，因此这个计划短暂的被搁置了，如果您知道方法，欢迎在[中文版本](xiaoditx.girhub.io/public/docs/ftxui%E4%B8%AD%E6%96%87%E7%BF%BB%E8%AF%91/installation/conan/)的下方留下评论以提醒我

---

<div class="section_buttons">

| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |

</div>