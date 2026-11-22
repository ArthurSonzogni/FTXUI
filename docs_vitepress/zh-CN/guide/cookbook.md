# 实战手册

使用 FTXUI 构建应用程序时的常见开发模式。

## 1. 后台线程与实时更新

在 `App::Loop()` 渲染期间，切勿在后台线程中直接修改 UI 状态。请使用线程安全的 `App::Post()` 将状态更新投递到主循环线程执行，再调用 `App::RequestAnimationFrame()` 触发重绘：

```cpp
#include <atomic>
#include <chrono>
#include <thread>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  auto app = App::Fullscreen();
  int progress = 0;

  auto ui = Renderer([&] {
    auto label = text("Downloading: " + std::to_string(progress) + "%");
    auto bar = gauge(progress / 100.f);
    return vbox({label, bar})
           | border
           | center;
  });

  std::atomic<bool> running = true;
  std::thread worker([&] {
    while (running && progress < 100) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      app.Post([&] { progress++; });
      app.RequestAnimationFrame();
    }
  });

  app.Loop(ui);
  running = false;
  worker.join();
}
```

## 2. 动态组件列表

当 `std::vector` 中的数据项增删时，使用 `DetachAllChildren()` 和 `Add(...)` 重建容器的子组件（务必在主循环线程中执行，例如在按钮回调或 `app.Post` 里）：

```cpp
std::vector<std::string> items = {"Item 1", "Item 2"};
auto list_container = Container::Vertical({});

auto rebuild_list = [&] {
  list_container->DetachAllChildren();
  for (size_t i = 0; i < items.size(); ++i) {
    auto item_button = Button(items[i], [&, i] {
      items.erase(items.begin() + static_cast<ptrdiff_t>(i));
      // Defer rebuild so we don't destroy the button while its callback runs.
      app.Post(rebuild_list);
    });
    list_container->Add(item_button);
  }
};
rebuild_list();
```

## 3. 支持鼠标滚轮的可滚动面板

`vscroll_indicator` 本身只是一个被动的滚动条指示器。要让面板支持鼠标滚轮和键盘滚动，需要将 `yframe`、`vscroll_indicator` 与 `focusPositionRelative`、`CatchEvent` 搭配使用：

```cpp
float scroll_y = 0.0f;

auto pane = Renderer([&] {
  Elements lines;
  for (int i = 0; i < 100; ++i) {
    lines.push_back(text("Line " + std::to_string(i)));
  }
  return vbox(std::move(lines))
         | focusPositionRelative(0.f, scroll_y)
         | vscroll_indicator
         | yframe
         | border;
});

pane |= CatchEvent([&](Event event) {
  if (event.is_mouse()) {
    if (event.mouse().button == Mouse::WheelDown) {
      scroll_y = std::min(1.0f, scroll_y + 0.05f);
      return true;
    }
    if (event.mouse().button == Mouse::WheelUp) {
      scroll_y = std::max(0.0f, scroll_y - 0.05f);
      return true;
    }
  }
  if (event == Event::ArrowDown) {
    scroll_y = std::min(1.0f, scroll_y + 0.05f);
    return true;
  }
  if (event == Event::ArrowUp) {
    scroll_y = std::max(0.0f, scroll_y - 0.05f);
    return true;
  }
  return false;
});
```

## 4. 全局快捷键与自定义 `Ctrl+C`

在根组件上挂载 `CatchEvent` 即可实现全局快捷键。返回 `true` 表示消费该事件，返回 `false` 则继续向子组件传递：

```cpp
auto app = App::Fullscreen();
app.ForceHandleCtrlC(false);  // Allow your component to intercept Ctrl+C.

auto root = main_ui
            | CatchEvent([&](Event event) {
                if (event == Event::Character('q') || event == Event::CtrlC) {
                  app.Exit();
                  return true;
                }
                return false;
              });
```

## 5. 模态框与确认对话框

使用 `Modal(dialog, &show_modal)` 包装主组件，并在对话框上附加 `CatchEvent` 以支持按 `Escape` 关闭：

```cpp
bool show_modal = false;

auto main_ui = Button("Delete item", [&] { show_modal = true; })
               | center;

auto confirm_button = Button("Confirm", [&] { show_modal = false; });
auto cancel_button = Button("Cancel", [&] { show_modal = false; });
auto modal_buttons = Container::Horizontal({
    confirm_button,
    cancel_button,
});

auto modal_layout = Renderer(modal_buttons, [&] {
  return vbox({
             text("Are you sure?") | bold,
             separator(),
             modal_buttons->Render(),
         })
         | border
         | clear_under;
});

auto modal = modal_layout
             | CatchEvent([&](Event e) {
                 if (e == Event::Escape) {
                   show_modal = false;
                   return true;
                 }
                 return false;
               });

auto root = main_ui
            | Modal(modal, &show_modal);
```

## 6. 鼠标命中检测与元素相对坐标

在 `Render()` 期间通过 `| reflect(box)` 捕获元素渲染后的边界 `Box`，然后在 `CatchEvent` 中检测鼠标点击位置：

```cpp
Box canvas_box;
int last_x = -1;
int last_y = -1;

auto ui = Renderer([&] {
  auto label = text("Click inside me (" + std::to_string(last_x) + ", " +
                    std::to_string(last_y) + ")");
  return label
         | border
         | reflect(canvas_box);
});

ui |= CatchEvent([&](Event event) {
  if (event.is_mouse() &&
      event.mouse().button == Mouse::Left &&
      event.mouse().motion == Mouse::Pressed &&
      canvas_box.Contain(event.mouse().x, event.mouse().y)) {
    last_x = event.mouse().x - canvas_box.x_min;
    last_y = event.mouse().y - canvas_box.y_min;
    return true;
  }
  return false;
});
```

## 7. 流式 CLI 输出与启动外部编辑器

使用 `App::TerminalOutput()` 构建内联 CLI，用 `PrintAbove()` 输出永久性的滚动日志，并用 `WithRestoredIO()` 临时将终端控制权交还给外部程序：

```cpp
auto app = App::TerminalOutput();

auto log_button = Button("Log line", [&] {
  app.PrintAbove(text("[info] Task completed") | color(Color::Green));
});
auto editor_button = Button("Open editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
auto quit_button = Button("Quit", app.ExitLoopClosure());

auto ui = Container::Horizontal({
    log_button,
    editor_button,
    quit_button,
});

app.Loop(ui);
```
