# Component 模組（`ftxui/component`）

**Component** 模組建立在 `ftxui/dom` 與 `ftxui/screen` 之上，用於打造具互動性且有狀態的終端機應用程式。

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
```

<FtxuiDemo file="component/homescreen" height="520px" title="Live demo: component/homescreen (click tabs or use arrow keys)" />

## 內建互動元件

所有內建元件都透過指標或 `Ref<T>` 直接綁定到你的普通 C++ 狀態變數：

| 元件 | 快速範例 | 線上範例 |
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

每個元件也都接受對應的 `*Option` 結構（例如 `ButtonOption::Animated()`、`MenuOption::HorizontalAnimated()`、`InputOption`），可用來設定樣式、動畫與回呼函式（`on_change`、`on_enter`、`transform`）。

## 組合版面配置與事件（`Container` + `Renderer`）

在 FTXUI 中，**容器（Container）**負責管理鍵盤與滑鼠事件路由及焦點，而 **`Renderer`** 則負責控制視覺版面配置：

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

### 容器種類

- `Container::Vertical({...})`：使用上下方向鍵與 Tab 垂直移動焦點。
- `Container::Horizontal({...})`：使用左右方向鍵與 Tab 水平移動焦點。
- `Container::Tab({...}, &active_tab)`：僅渲染目前作用中的子元件並將事件路由給它。
- `Container::Stacked({...})`：依序渲染所有子元件（適合用於浮動的 `Window` 視窗面板）。

## 元件裝飾器

元件與 DOM 元素一樣，都能透過 `|` 運算子進行組合：

- `CatchEvent(fn)`：在子元件收到事件前先行攔截（回傳 `true` 表示消耗，回傳 `false` 表示繼續向上／向下傳遞）。詳見[事件、焦點與滑鼠](/zh-TW/guide/events-and-focus)。
- `Maybe(&show)`：依條件顯示或隱藏元件，並在隱藏時停用其事件處理。
- `Modal(dialog, &show)`：當 `show` 為 `true` 時，將 `dialog` 疊加在主介面之上並阻擋背景事件。詳見 [`component/modal_dialog`](/examples/component/modal_dialog)。
- `Hoverable(&is_hovered)`：追蹤滑鼠游標目前是否懸停在元件上方。

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

## 從背景執行緒更新介面

`App` 只會在收到事件或被請求繪製新幀時才重繪畫面。若要從背景工作執行緒安全地更新 UI 狀態，請先呼叫 `app.Post(...)`（會在主迴圈執行緒中執行），接著呼叫 `app.RequestAnimationFrame()`：

```cpp
std::thread worker([&] {
  auto result = fetch_data();
  app.Post([&, result = std::move(result)] {
    data = result;
  });
  app.RequestAnimationFrame();
});
```

## 在行內應用程式上方輸出訊息（`PrintAbove`）

在 `App::TerminalOutput()` 模式下，互動式 UI 會直接繪製在先前的 shell 輸出下方。呼叫執行緒安全的 `app.PrintAbove(element)`，即可在即時提示介面上方附加永久保留的日誌或對話訊息：

```cpp
auto app = App::TerminalOutput();
app.PrintAbove(paragraph("Build finished!") | color(Color::Green));
```

參閱 [`component/print_above`](/examples/component/print_above)。

## 自訂事件迴圈（`Loop`）

如果你想把 FTXUI 整合進既有的遊戲迴圈或網路事件反應器（reactor），可以改用 [`ftxui::Loop`](/api/class/loop) 取代 `app.Loop(...)`：

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

參閱 [`component/custom_loop`](/examples/component/custom_loop)。

## 暫時還原終端機 I/O（`WithRestoredIO`）

若要暫時退出終端機原始模式（raw mode）並啟動外部程式（例如 `$EDITOR`、`less` 或 `git`），請用 `app.WithRestoredIO(...)` 包裝該呼叫：

```cpp
auto edit_btn = Button("Open Editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
```

參閱 [`component/with_restored_io`](/examples/component/with_restored_io)。

## 下一步

- [事件、焦點與滑鼠](/zh-TW/guide/events-and-focus)：事件路由、焦點管理與滑鼠點擊偵測
- [自訂元件](/zh-TW/guide/custom-widgets)：繼承 `ComponentBase` 與 `Node`
- [`ftxui/component` API 參考手冊](/api/component)：完整的函式與選項列表
