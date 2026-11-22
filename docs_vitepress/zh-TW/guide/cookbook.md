# 實用食譜

使用 FTXUI 開發應用程式時的常見實作模式。

## 1. 背景執行緒與即時更新

當 `App::Loop()` 正在渲染畫面時，絕對不要從背景執行緒直接修改 UI 狀態。請改用執行緒安全的 `App::Post()` 將狀態更新排程到主迴圈執行緒執行，並呼叫 `App::RequestAnimationFrame()` 觸發重繪：

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

## 2. 動態元件清單

當 `std::vector` 中的項目增加或減少時，請使用 `DetachAllChildren()` 與 `Add(...)` 重建容器的子元件（務必在主迴圈執行緒中執行，例如在按鈕回呼函式內或透過 `app.Post`）：

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

## 3. 支援滑鼠滾輪的可捲動面板

`vscroll_indicator` 只是一個被動的視覺指示器。若要讓面板能透過滑鼠滾輪與鍵盤捲動，請將 `yframe`、`vscroll_indicator` 搭配 `focusPositionRelative` 與 `CatchEvent` 使用：

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

## 4. 全域快捷鍵與自訂 `Ctrl+C`

在根元件（root component）上掛載 `CatchEvent` 來實作全域快捷鍵。回傳 `true` 表示已消耗該事件，回傳 `false` 則讓事件繼續傳遞給子元件：

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

## 5. 互動視窗（Modal）與確認對話框

使用 `Modal(dialog, &show_modal)` 包裝你的主元件，並在對話框上加上 `CatchEvent` 以便按下 `Escape` 時關閉：

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

## 6. 滑鼠點擊偵測與元件相對座標

在 `Render()` 期間透過 `| reflect(box)` 取得元素渲染後的邊界 `Box`，接著在 `CatchEvent` 裡檢查滑鼠點擊座標：

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

## 7. 串流 CLI 輸出與啟動外部編輯器

使用 `App::TerminalOutput()` 建立行內 CLI，透過 `PrintAbove()` 印出會保留在終端機捲動歷史中的訊息，並利用 `WithRestoredIO()` 暫時將終端機控制權交還給外部程式：

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
