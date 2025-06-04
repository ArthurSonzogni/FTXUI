@page installation_cmake CMake

@tableofcontents

This page explains how to depend on FTXUI using [CMake](https://cmake.org).  

# Methods of Integration

## Using FetchContent

This approach downloads FTXUI at configure time and doesn't require a system-wide install.

```cmake 
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v6.1.9  # Replace with a version, tag, or commit hash
)

FetchContent_MakeAvailable(ftxui)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

This ensures reproducible builds and easy dependency management.

## Using find_package

If FTXUI is installed system-wide or via a package manager (e.g. vcpkg or Conan), you can use:

```cmake 
find_package(ftxui REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

Make sure the package is visible in your `CMAKE_PREFIX_PATH`.

## Using git submodule

You can also add FTXUI as a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules), keeping it as part of your repository:

```cmake
git submodule add https://github.com/ArthurSonzogni/FTXUI external/ftxui
git submodule update --init --recursive
```

When cloning a repository that already includes FTXUI as a submodule, make sure to fetch submodules with:

```
git clone --recurse-submodules <your-repo>
# Or, if already cloned:
git submodule update --init --recursive
```

Then in your `CMakeLists.txt`:

```cmake
add_subdirectory(external/ftxui)

add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

This approach works well if you want to vendor FTXUI in your own repository.

# Optional CMake Flags

FTXUI supports the following CMake options:

| Option                            | Description                   | Default |
| --------------------------------- | ----------------------------- | ------- |
| FTXUI_BUILD_EXAMPLES              | Build bundled examples        | OFF     |
| FTXUI_BUILD_DOCS                  | Build the documentation       | OFF     |
| FTXUI_BUILD_TESTS                 | Enable tests                  | OFF     |
| FTXUI_ENABLE_INSTALL              | Generate install targets      | ON      |
| FTXUI_MICROSOFT_TERMINAL_FALLBACK | Improve Windows compatibility | ON/OFF  |

To enable an option:

```
cmake -DFTXUI_BUILD_EXAMPLES=ON ..
```

# Verifying Integration

To confirm the setup is working, build and run a minimal example.
If you need a complete template, see: [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter)

<div class="section_buttons">
 
| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |
 
</div>
