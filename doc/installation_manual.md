@page installation_manual Manual
@tableofcontents

## Building from Source (Official)

Clone and build the project using CMake:

```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -S . -B build -DFTXUI_ENABLE_INSTALL=ON
cmake --build build -j
sudo cmake --install build
```

Link the libraries in your application:

```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```

<div class="section_buttons">

| Previous          |
|:------------------|
| [Getting Started](getting-started.html) |

</div>
