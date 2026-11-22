# 事件、焦点与鼠标

本篇介绍 FTXUI 如何路由键盘事件、在组件间切换焦点以及处理鼠标输入。

## 事件如何在组件树中传播

当按键、鼠标操作或终端尺寸变化事件到达时：

1. `App` 通过 `root->OnEvent(event)` 将 `Event` 派发给**根组件**。
2. 容器（`Container::Vertical`、`Container::Horizontal`、`Container::Tab`、`Container::Stacked`）会先将键盘事件转发给**当前激活的子组件**。
3. 如果子组件返回 `true`，表示事件已被**消费**，传播随即停止。
4. 如果子组件返回 `false`，容器会自行处理导航按键（例如 `Container::Vertical` 中的 `ArrowUp`/`ArrowDown`、`Container::Horizontal` 中的 `ArrowLeft`/`ArrowRight`，以及在可聚焦子组件间切换的 `Tab`/`TabReverse`）。

```text
  App → root->OnEvent(e)
           │
           ▼
     active child->OnEvent(e)
       ├── returns true  → done (event consumed)
       └── returns false → container handles navigation / bubbles up
```

## 使用 `CatchEvent` 拦截事件

`CatchEvent` 会包装目标组件，并在每个 `Event` 到达内部组件**之前**先行检查：

```cpp
auto app = App::Fullscreen();

auto ui = main_component
          | CatchEvent([&](Event event) {
              if (event == Event::Character('q')) {
                app.Exit();
                return true;  // Consumed: inner component won't see 'q'
              }
              return false;   // Not consumed: pass to main_component
            });
```

::: warning 全局快捷键与 `Input` 的冲突
由于 `CatchEvent` 在子组件**之前**执行，如果在根组件拦截 `Event::Character('q')`，会导致用户在 `Input` 输入框中无法输入 `'q'`！解决方法如下：
- 使用带修饰键的组合键（如 `Event::CtrlQ`、`Event::Escape`），或者
- 在消费可打印字符前检查 `!input->Focused()`。
:::

### 常见 `Event` 判断

```cpp
if (event == Event::Return) { /* Enter */ }
if (event == Event::Escape) { /* Esc */ }
if (event == Event::CtrlC)  { /* Ctrl+C */ }
if (event.is_character())   { std::string ch = event.character(); }
if (event.is_mouse())       { Mouse m = event.mouse(); }
```

## 焦点与容器

- 只有 `Focusable()` 返回 `true` 的组件才能获取焦点。没有可聚焦子组件的 `Renderer(...)` 本身不可聚焦。
- **`Container::Vertical` / `Container::Horizontal`** 会自动维护当前激活子组件的索引（也可以显式传入 `int* selected` 指针来观察或控制焦点）。
- `Container::Tab({tab0, tab1, tab2}, &tab_index)` 仅将事件路由到 `children[tab_index]`。非激活标签页既不会收到事件，也不会获取焦点。
- **通过代码聚焦**：调用 `child->TakeFocus()` 可直接将焦点移动到指定组件。
- **查询焦点状态**：在 `Renderer` 中，可调用 `child->Focused()`（当前叶子组件拥有焦点）或 `child->Active()`（该组件或其某个后代拥有焦点）来高亮激活面板。

```cpp
auto pane = Renderer(child, [&] {
  auto border_style = child->Active() ? borderDouble : borderLight;
  return child->Render() | border_style;
});
```

## 模态框与事件隔离

`Modal(dialog, &show)` 会将 `dialog` 叠加在主界面上方（底层使用 `dbox` + `clear_under`），**并且**在 `show == true` 期间阻止所有键盘和鼠标事件传递到背景组件：

```cpp
bool show_modal = false;
auto ui = main_component
          | Modal(dialog_component, &show_modal);
```

当 `show_modal` 变为 `false` 时，事件会立即恢复路由到 `main_component`。

## 鼠标事件与命中检测

`App` 默认自动启用鼠标支持。`Mouse` 结构体包含以下字段：

- `mouse.x`、`mouse.y`：从 0 开始的终端单元格坐标
- `mouse.button`：`Mouse::Left`、`Mouse::Middle`、`Mouse::Right`、`Mouse::None`、`Mouse::WheelUp`、`Mouse::WheelDown`
- `mouse.motion`：`Mouse::Pressed`、`Mouse::Released`、`Mouse::Moved`
- 修饰键：`mouse.shift`、`mouse.meta`、`mouse.control`

要在自定义区域上检测点击或悬停，可在 `Render()` 期间用 `reflect(box)` 捕获渲染后的矩形区域，再在 `OnEvent()` 中调用 `box.Contain(x, y)` 判断：

```cpp
Box card_box;
bool hovered = false;

auto card = Renderer([&] {
  auto style = hovered ? color(Color::Yellow) : color(Color::White);
  return text(" Click or hover me ")
         | style
         | borderRounded
         | reflect(card_box);
});

auto interactive_card = card
                        | CatchEvent([&](Event event) {
                            if (!event.is_mouse()) {
                              return false;
                            }
                            hovered = card_box.Contain(event.mouse().x, event.mouse().y);
                            if (hovered &&
                                event.mouse().button == Mouse::Left &&
                                event.mouse().motion == Mouse::Pressed) {
                              // Handle click
                              return true;
                            }
                            return false;
                          });
```

::: tip 内置的 `Hoverable`
如果只需要一个表示鼠标悬停状态的布尔值，直接使用内置装饰器即可，无需手写命中检测：
```cpp
bool is_hovered = false;
auto card = base_component | Hoverable(&is_hovered);
```
:::
