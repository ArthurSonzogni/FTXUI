# C++20 模块 (Modules)

::: warning 实验性功能
该特性目前仍在开发中，API 在后续版本中可能会发生变化。如果你遇到任何问题或有改进建议，欢迎在 GitHub 上提交 Issue。
:::

FTXUI 实验性支持 [C++20 模块](https://en.cppreference.com/w/cpp/language/modules)，以缩短编译时间并改善代码组织。库的每个部分都有对应的模块，并按头文件拆分为模块分区（partition）。

构建 FTXUI 时开启 `FTXUI_BUILD_MODULES` 选项即可生成 C++20 模块，例如配合 CMake 与 Ninja：

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```

::: info 注意
使用 C++20 模块需要支持 C++20 的编译器、CMake 3.20 及以上版本，以及兼容的构建生成器（如 Ninja）。Makefile 生成器不支持 C++20 模块。
:::

随后就可以在自己的代码中直接导入模块使用：

```cpp
import ftxui;

using ftxui::App;
using ftxui::Button;
using ftxui::Component;

int main() {
  App app = App::TerminalOutput();
  Component button = Button("Click me", app.ExitLoopClosure());
  app.Loop(button);
  return 0;
}
```

写下 `import ftxui;` 等价于包含所有 `<ftxui/**/*.hpp>` 头文件，通过单个模块即可引入整个库。

要在 CMake 中正确查找并链接模块，请使用 `target_link_libraries`，以自动获取所需的编译器和链接器标志：

```cmake
target_link_libraries(my_executable
    #...whatever...
    PRIVATE ftxui::modules
)
```

### 模块列表

虽然 `import ftxui;` 提供了完整的库功能，但 FTXUI 本身是分层设计的。如果只需要其中某一部分，也可以直接导入对应的独立模块：

- `ftxui`（便捷聚合模块，重新导出以下所有模块）
    - `ftxui.component`（交互式组件、事件与事件循环）
    - `ftxui.dom`（基于 Element 的布局与样式）
    - `ftxui.screen`（终端渲染、单元格与颜色）
    - `ftxui.util`（内部工具）

例如：
```cpp
import ftxui.screen;
import ftxui.dom;

// Use only screen and dom functionalities...
```
