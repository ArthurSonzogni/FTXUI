@page installation_meson Meson

@tableofcontents

FTXUI supports [Meson](https://mesonbuild.com/) as an alternative build system.

## Prerequisites

- C++17 compatible compiler (GCC >= 7, Clang >= 5, MSVC >= 2017)
- [Meson](https://mesonbuild.com/Getting-meson.html) >= 1.1.0
- [Ninja](https://ninja-build.org/) (recommended backend)

## Quick Start

```bash
# Configure (library only)
meson setup builddir

# Build
ninja -C builddir

# Install
ninja -C builddir install
```

## Build Options

| Option      | Default | Description                |
|-------------|---------|----------------------------|
| `examples`  | `false` | Build example applications |
| `tests`     | `false` | Build and run tests        |

### Build with examples

```bash
meson setup builddir -Dexamples=true
ninja -C builddir
```

### Build with tests

```bash
meson setup builddir -Dtests=true
ninja -C builddir
meson test -C builddir
```

### Build everything

```bash
meson setup builddir -Dexamples=true -Dtests=true
ninja -C builddir
meson test -C builddir
```

## Dependencies

### Google Test (tests only)

When tests are enabled, FTXUI depends on [Google Test](https://github.com/google/googletest) (v1.17.0).

The dependency resolution follows the same strategy as the CMake build:

1. **System-installed gtest**: If `gtest` is available via `pkg-config` or system paths, it will be used directly.
2. **Automatic download (fallback)**: If not found on the system, Meson will automatically download gtest v1.17.0 via the [WrapDB](https://mesonbuild.com/Wrapdb-projects.html) using the `subprojects/gtest.wrap` file.

No manual intervention is required — Meson handles this transparently.

## Library Targets

The build produces three libraries:

| Library            | Description                          |
|--------------------|--------------------------------------|
| `libftxui-screen`  | Terminal rendering and input         |
| `libftxui-dom`     | Layout and styling elements          |
| `libftxui-component` | Interactive UI components          |

Dependencies between them:
```
component → dom → screen
```

## Using FTXUI as a Meson Subproject

To use FTXUI in your own Meson project, create a wrap file at `subprojects/ftxui.wrap`:

```ini
[wrap-git]
url = https://github.com/ArthurSonzogni/FTXUI.git
revision = v7.0.0

[provide]
ftxui-screen = ftxui_screen_dep
ftxui-dom = ftxui_dom_dep
ftxui-component = ftxui_component_dep
```

Then in your `meson.build`:

```meson
ftxui_component_dep = dependency('ftxui-component',
  fallback: ['ftxui', 'ftxui_component_dep'],
)

executable('my_app',
  'main.cpp',
  dependencies: ftxui_component_dep,
)
```

## Reconfiguring

To change options after initial configuration:

```bash
meson configure builddir -Dtests=true
ninja -C builddir
```

## Clean Build

```bash
rm -rf builddir
meson setup builddir
```
