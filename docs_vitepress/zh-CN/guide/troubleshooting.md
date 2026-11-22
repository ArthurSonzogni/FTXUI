# 故障排查与常见问题

这里汇集了构建、运行与版本迁移过程中最常见的问题及解决方案。

## 子组件收不到任何事件

**现象**：按钮、输入框或菜单都能正常显示在屏幕上，但键盘和鼠标操作毫无反应。

**原因**：使用了 `Renderer([&] { return child->Render(); })`，但忘记把 `child`（或包含所有子组件的 `Container`）作为第一个参数传入。

**解决方法**：务必将 `Container` 与 `Renderer(container, ...)` 配对使用：
```cpp
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({input->Render(), button->Render()})
         | border;
});
```

详见[心智模型](/zh-CN/guide/mental-model)。

---

## 手动链接时出现未定义引用 (Undefined References)

**现象**：链接阶段报错，提示找不到 [`ftxui::Node`](/api/class/node)、[`ftxui::Screen`](/api/class/screen) 或 [`ftxui::Element`](/api/component#element) 等符号。

**解决方法**：使用静态库时，链接顺序非常重要（`component` 依赖 `dom`，`dom` 依赖 `screen`）：
```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```
如果使用 CMake，直接链接 `ftxui::ftxui`（或 `ftxui::component`，它会自动传递依赖 `dom` 和 `screen`）即可。

---

## 在 Linux 上使用 MinGW 交叉编译报错 (`'mutex' is not a member of 'std'`)

**现象**：在 Debian/Ubuntu 上使用 `x86_64-w64-mingw32-g++` 编译时报错 `error: 'mutex' is not a member of 'std'`。

**解决方法**：Debian/Ubuntu 默认的 MinGW 编译器采用 `win32` 线程模型，未实现 C++11 的 `<mutex>`/`<thread>`。请切换到 `posix` 变体：
- 在 CMake 工具链文件中将 `CMAKE_CXX_COMPILER` 设为 `x86_64-w64-mingw32-g++-posix`，或者
- 运行 `sudo update-alternatives --config x86_64-w64-mingw32-g++` 并选择 `x86_64-w64-mingw32-g++-posix`。

---

## Windows 宏冲突 (`RGB`、`DrawText`、`min`、`max`)

**现象**：包含 `<windows.h>` 后，在 `<ftxui/screen/color.hpp>` 或 `<ftxui/dom/canvas.hpp>` 中出现奇怪的编译错误。

**解决方法**：在包含 `<windows.h>` 之前定义 `NOMINMAX` 和 `WIN32_LEAN_AND_MEAN`（或在 CMake 中传入 `-DNOMINMAX`）。FTXUI 头文件内部已经取消了 `RGB` 和 `DrawText` 宏的定义，而定义 `NOMINMAX` 可以避免与 `std::min`/`std::max` 发生冲突。

---

## 终端中颜色、Unicode 或鼠标无法正常工作

- **手动覆盖色彩支持检测**：
  ```cpp
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  ```
- **禁用 Unicode 方块字符 / 退回纯 ASCII 渲染**：
  ```cpp
  auto quirks = Terminal::GetQuirks();
  quirks.SetComponentAscii(true);
  Terminal::SetQuirks(quirks);
  ```
- **Windows 7 / 旧版控制台**：Windows 10 之前的旧版 `conhost` 不支持虚拟终端转义序列（`ENABLE_VIRTUAL_TERMINAL_PROCESSING`）。请改用 Windows Terminal 或其他现代 VT100 兼容终端。

---

## 从 FTXUI v6 迁移到 v7

为了语义更清晰，FTXUI v7 重命名了三个核心类（旧名称仍以废弃别名形式保留，保证向后兼容）：

| FTXUI v6（已废弃） | FTXUI v7 | 头文件 |
| --- | --- | --- |
| `ScreenInteractive` | `App` | `<ftxui/component/app.hpp>` |
| `Pixel` / `PixelAt()` | `Cell` / `CellAt()` | `<ftxui/screen/cell.hpp>` |
| `Image` / `DrawImage()` | `Surface` / `DrawSurface()` | `<ftxui/screen/surface.hpp>` |
