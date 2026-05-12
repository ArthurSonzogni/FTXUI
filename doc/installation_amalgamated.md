@page installation_amalgamated Amalgamated
@tableofcontents

## Amalgamated version

The amalgamated version is a single-header (`ftxui.hpp`) and single-source
(`ftxui.cpp`) version of the library. It is an alternative way to integrate
FTXUI into your project, especially if you don't want to use a package manager
or a complex build system.

A truly single-file header-only version (`ftxui_all.hpp`) is also available.

### How to get it?

You can get the amalgamated files in several ways:

1. **GitHub Releases (Recommended)**: Download the `ftxui-amalgamated.zip`
   from the [latest release](https://github.com/ArthurSonzogni/FTXUI/releases).
   *Note: This will be officially available starting from version 7.0.0. For 
   older versions or before the 7.0.0 release, please use the CI artifacts 
   below.*
   It contains `ftxui.hpp`, `ftxui.cpp`, and the header-only `ftxui_all.hpp`.
2. **CI Artifacts**: If you want the absolute latest version from `main` (or 
   before the official 7.0.0 release), you can download the artifacts from the 
   [Amalgamate workflow](https://github.com/ArthurSonzogni/FTXUI/actions/workflows/amalgamate.yaml).
3. **Manual Generation**: Run the following command from the root of the
   cloned repository:
   ```bash
   ./tools/amalgamate.py
   ```

### How to use it?

#### Option 1: Split Header/Source (Recommended for build speed)

Copy `ftxui.hpp` and `ftxui.cpp` into your project. Include `ftxui.cpp` in your
source list.

#### Option 2: Header-Only

Copy `ftxui_all.hpp` into your project. In **exactly one** C++ file, define
`FTXUI_IMPLEMENTATION` before including the header:

```cpp
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

In other files, you can simply include it without the define:

```cpp
#include "ftxui_all.hpp"
```

### Compilation

When compiling your project, ensure you are using C++17 and linking against
threads.

```bash
g++ -std=c++17 your_main.cpp [ftxui.cpp] -o your_app -lpthread
```

<div class="section_buttons">

| Previous          |
|:------------------|
| [Installation](installation.html) |

</div>
