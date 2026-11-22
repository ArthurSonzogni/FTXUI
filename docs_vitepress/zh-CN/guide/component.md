# Component 模块 (`ftxui/component`)

**Component** 模块构建在 `ftxui/dom` 和 `ftxui/screen` 之上，用于开发交互式、有状态的终端应用程序。

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
```

<FtxuiDemo file="component/homescreen" height="520px" title="Live demo: component/homescreen (click tabs or use arrow keys)" />

## 内置交互式控件

所有内置组件都通过指针或 `Ref<T>` 直接绑定到你的普通 C++ 状态变量：

| 组件 | 快速示例 | 在线演示 |
| --- | --- | --- |
| `Button` | `auto btn = Button("Click", [&] { count++; });` | [`component/button`](/examples/component/button) |
| `Input` | `auto in = Input(&text_val, "placeholder");` | [`component/input`](/examples/component/input) |
| `Checkbox` | `auto cb = Checkbox("Enable", &enabled);` | [`component/checkbox`](/examples/component/checkbox) |
| `Menu` | `auto m = Menu(&entries, &selected);` | [`component/menu`](/examples/component/menu) |
| `Toggle` | `auto tg = Toggle(&entries, &selected);` | [`component/toggle`](/examples/component/toggle) |
| `Radiobox` | `auto rb = Radiobox(&entries, &selected);` | [`component/radiobox`](/examples/component/radiobox) |
| `Dropdown` | `auto dd = Dropdown(&entries, &selected);` | [`component/dropdown`](/examples/component/dropdown) |
| `Slider` | `auto sl = Slider("Volume:", &val, 0, 100, 5);` | [`component/slider`](/examples/component/slider) |
| `Collapsible` | `auto col = Collapsible("Details", child);` | [`component/collapsible`](/examples/component/collapsible) |
| `Window` | `auto win = Window({.inner = child, .title = "Title"});` | [`component/window`](/examples/component/window) |
| `ResizableSplit` | `ResizableSplitLeft(left, right, &split_size)` | [`component/resizable_split`](/examples/component/resizable_split) |

每个控件还支持传入对应的 `*Option` 结构体（例如 `ButtonOption::Animated()`、`MenuOption::HorizontalAnimated()`、`InputOption`），用于定制样式、动画以及回调函数（`on_change`、`on_enter`、`transform`）。

## 组合布局与事件 (`Container` + `Renderer`)

在 FTXUI 中，**容器**负责管理键盘/鼠标事件路由与焦点，**`Renderer`** 则负责控制视觉布局：

```cpp
std::string username;
std::string password;

auto input_user = Input(&username, "username");
InputOption password_option;
password_option.password = true;
auto input_pass = Input(&password, "password", password_option);
auto submit = Button("Sign in", [&] { app.Exit(); });

auto form = Container::Vertical({
    input_user,
    input_pass,
    submit,
});

auto ui = Renderer(form, [&] {
  auto header = text("Login") | bold | hcenter;
  auto user_row = hbox({text("User: "), input_user->Render()});
  auto pass_row = hbox({text("Pass: "), input_pass->Render()});
  auto btn_row = submit->Render() | hcenter;

  return vbox({
             header,
             separator(),
             user_row,
             pass_row,
             separator(),
             btn_row,
         })
         | borderRounded;
});
```

### 容器类型

- `Container::Vertical({...})`：使用上下方向键和 Tab 在垂直方向切换焦点。
- `Container::Horizontal({...})`：使用左右方向键和 Tab 在水平方向切换焦点。
- `Container::Tab({...}, &active_tab)`：仅渲染当前激活的子组件并向其派发事件。
- `Container::Stacked({...})`：按顺序叠层渲染所有子组件（适合浮动 `Window` 窗口）。

## 组件装饰器

组件与 DOM 元素一样，支持使用 `|` 操作符进行组合：

- `CatchEvent(fn)`：在事件到达子组件前拦截处理（返回 `true` 消费事件，返回 `false` 继续冒泡）。详见[事件、焦点与鼠标](/zh-CN/guide/events-and-focus)。
- `Maybe(&show)`：按条件控制组件是否显示，隐藏时同时禁用其事件处理。
- `Modal(dialog, &show)`：在主界面上方叠加 `dialog`，并在 `show` 为 `true` 时拦截背景事件。参见 [`component/modal_dialog`](/examples/component/modal_dialog)。
- `Hoverable(&is_hovered)`：追踪鼠标指针是否正悬停在组件上。

```cpp
bool show_help = false;

auto ui = main_component
          | Modal(help_dialog, &show_help)
          | CatchEvent([&](Event event) {
              if (event == Event::Character('?')) {
                show_help = !show_help;
                return true;
              }
              return false;
            });
```

## 从后台线程更新界面

`App` 只在收到事件或显式请求新帧时才会重绘。要从工作线程安全地更新 UI 状态，请先调用 `app.Post(...)`（会在主循环线程上执行），再调用 `app.RequestAnimationFrame()`：

```cpp
std::thread worker([&] {
  auto result = fetch_data();
  app.Post([&, result = std::move(result)] {
    data = result;
  });
  app.RequestAnimationFrame();
});
```

## 在内联应用上方输出日志 (`PrintAbove`)

在 `App::TerminalOutput()` 模式下，交互界面会内联绘制在已有终端输出的下方。可以调用线程安全的 `app.PrintAbove(element)`，在实时交互界面上方追加永久性的日志或聊天消息：

```cpp
auto app = App::TerminalOutput();
app.PrintAbove(paragraph("Build finished!") | color(Color::Green));
```

参见 [`component/print_above`](/examples/component/print_above)。

## 自定义事件循环 (`Loop`)

如果需要将 FTXUI 集成到已有的游戏循环或网络反应器（reactor）中，可以使用 [`ftxui::Loop`](/api/class/loop) 替代阻塞式的 `app.Loop(...)`：

```cpp
#include <ftxui/component/loop.hpp>

auto app = App::Fullscreen();
Loop loop(&app, ui);

while (!loop.HasQuitted()) {
  update_simulation();
  app.RequestAnimationFrame();
  loop.RunOnce();
}
```

参见 [`component/custom_loop`](/examples/component/custom_loop)。

## 恢复终端 I/O (`WithRestoredIO`)

若要临时挂起终端原始模式（raw mode）并启动外部程序（如 `$EDITOR`、`less` 或 `git`），请使用 `app.WithRestoredIO(...)` 包装调用：

```cpp
auto edit_btn = Button("Open Editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
```

参见 [`component/with_restored_io`](/examples/component/with_restored_io)。

## 下一步

- [事件、焦点与鼠标](/zh-CN/guide/events-and-focus)：事件路由、焦点管理与鼠标命中检测
- [自定义控件](/zh-CN/guide/custom-widgets)：继承 `ComponentBase` 与 `Node`
- [`ftxui/component` API 参考](/api/component)：完整的函数与选项列表
