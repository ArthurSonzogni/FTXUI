# 心智模型

FTXUI 围绕几个核心理念设计。提前掌握这些概念，能让 API 用起来顺手得多。

## 1. 三个分层模块

FTXUI 划分为三个逐层构建的模块（`screen → dom → component`）：

- **`ftxui/screen`**（[指南](/zh-CN/guide/screen)、[API](/api/screen)）：底层带样式的二维字符网格（`Screen`、`Surface`、`Cell`、`Color`、`Box`）。
- **`ftxui/dom`**（[指南](/zh-CN/guide/dom)、[API](/api/dom)）：无状态布局树（`Element`、`hbox`、`vbox`、`border`、`table`、`canvas`、`Render()`）。
- **`ftxui/component`**（[指南](/zh-CN/guide/component)、[API](/api/component)）：交互式控件与事件循环（`Component`、`Button`、`Input`、`Menu`、`Container`、`App`）。

你也可以直接使用聚合目标（CMake 中的 `ftxui::ftxui`、Bazel 中的 `@ftxui//:ftxui`、头文件 `#include <ftxui/ftxui.hpp>`）一次性引入全部三个模块。

## 2. Element 与 Component 的区别

| | **Element** (`ftxui/dom`) | **Component** (`ftxui/component`) |
| --- | --- | --- |
| **生命周期** | 临时的，每一帧重新构建 | 持久的，在 `app.Loop(...)` 之前创建一次 |
| **状态** | 无状态的值树 | 持有焦点状态以及指向 C++ 状态变量的引用 |
| **输入** | 不接收事件 | 通过 `OnEvent` 处理键盘和鼠标 `Event` |
| **常见示例** | `text`、`hbox`、`vbox`、`border`、`gauge`、`table`、`canvas` | `Button`、`Input`、`Menu`、`Slider`、`Modal`、`Window`、`Container` |

在 FTXUI 中，UI 是 C++ 状态的函数。你不需要原地修改某个控件的标签文本，只需更新普通的 C++ 变量（`std::string`、`int`、`bool`、`std::vector`），让 `Render()` 在下一帧根据最新状态构建全新的 `Element` 树即可。

## 3. 事件树与视觉布局树

在 FTXUI 中，**事件/焦点路由**与**视觉布局**是有意分离的：

- **容器**（`Container::Vertical`、`Container::Horizontal`、`Container::Tab`、`Container::Stacked`）定义**事件与焦点树**（决定哪个子组件接收键盘输入，以及方向键和 `Tab` 如何切换焦点）。
- **`Renderer(container, [&] { ... })`** 定义**视觉布局**，同时把所有事件转发给 `container`。

::: danger 常见陷阱
在 `Renderer([&] { ... })` 中调用 `child->Render()` 时，如果**没有**把子组件（或包含所有子组件的容器）作为第一个参数传入，键盘和鼠标事件将**永远不会**送达这些子组件！
:::

```cpp
// WRONG: `input` and `button` are drawn, but never receive events!
auto ui = Renderer([&] {
  return vbox({input->Render(), button->Render()});
});

// RIGHT: `container` routes events & focus; `Renderer` controls layout.
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({
      text("Login") | bold | hcenter,
      separator(),
      input->Render(),
      button->Render(),
  }) | border;
});
```

注意，你不必在 lambda 里调用 `container->Render()`。直接调用各个子组件的 `->Render()`，就能自由地用 `vbox`、`hbox`、`window`、`gridbox` 等任意方式排布它们。

## 4. 帧生命周期

```text
  Terminal input / App::Post / RequestAnimationFrame
                     │
                     ▼
        root_component->OnEvent(event)
        (mutates your plain C++ state)
                     │
                     ▼
        Element tree = root_component->Render()
        (fresh stateless DOM tree from state)
                     │
                     ▼
        Render(screen, tree) → diff → terminal
```

- FTXUI 仅在收到事件或显式请求动画帧时才会重绘。
- 屏幕输出会与上一帧进行 **diff 增量对比**，并用同步更新序列（DEC mode 2026）包裹，避免闪烁和画面撕裂。

## 5. 选择合适的 `App` 模式

| 工厂方法 | 行为 | 适用场景 |
| --- | --- | --- |
| `App::Fullscreen()` | 使用备用屏幕缓冲区（占满整个终端，退出后恢复原内容）。 | 全屏 TUI（仪表盘、编辑器、文件管理器） |
| `App::TerminalOutput()` | 在已有终端输出下方内联绘制，支持 `PrintAbove()`。 | 交互式 CLI、提示符、进度条和日志工具 |
| `App::FitComponent()` | 自动按组件的最小宽高调整尺寸。 | 紧凑的嵌入式控件和快速交互提示 |
| `App::FixedSize(w, h)` | 固定宽度和高度。 | 单元测试和固定尺寸界面 |
