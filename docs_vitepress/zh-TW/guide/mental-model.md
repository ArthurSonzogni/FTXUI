# 心智模型

FTXUI 建立在幾個核心概念之上。先掌握這些概念，使用起 API 會順手許多。

## 1. 三層模組架構

FTXUI 拆分為三個層層堆疊的模組（`screen → dom → component`）：

- **`ftxui/screen`**（[指南](/zh-TW/guide/screen)、[API](/api/screen)）：底層的二維樣式字元格網格（`Screen`、`Surface`、`Cell`、`Color`、`Box`）。
- **`ftxui/dom`**（[指南](/zh-TW/guide/dom)、[API](/api/dom)）：無狀態的版面配置樹（`Element`、`hbox`、`vbox`、`border`、`table`、`canvas`、`Render()`）。
- **`ftxui/component`**（[指南](/zh-TW/guide/component)、[API](/api/component)）：互動式元件與事件迴圈（`Component`、`Button`、`Input`、`Menu`、`Container`、`App`）。

你也可以直接使用整合目標（CMake 的 `ftxui::ftxui`、Bazel 的 `@ftxui//:ftxui`、標頭檔 `#include <ftxui/ftxui.hpp>`）一次引入這三個模組。

## 2. Element 與 Component 的差異

| | **Element** (`ftxui/dom`) | **Component** (`ftxui/component`) |
| --- | --- | --- |
| **生命週期** | 短暫存在，每一幀重新建構 | 持續存在，在 `app.Loop(...)` 之前建立一次 |
| **狀態** | 無狀態的值樹（Value tree） | 保存焦點狀態，並持有你 C++ 狀態變數的指標或參考 |
| **輸入** | 不接收事件 | 透過 `OnEvent` 處理鍵盤與滑鼠的 `Event` |
| **範例** | `text`、`hbox`、`vbox`、`border`、`gauge`、`table`、`canvas` | `Button`、`Input`、`Menu`、`Slider`、`Modal`、`Window`、`Container` |

你的 UI 就是 C++ 狀態的函式。你不需要直接去修改元件上的標籤文字，只要更新普通的 C++ 變數（`std::string`、`int`、`bool`、`std::vector`），讓 `Render()` 在下一幀建立全新的 `Element` 樹即可。

## 3. 事件樹與視覺版面樹分離

在 FTXUI 中，**事件／焦點路由**與**視覺版面配置**是刻意分開的：

- **Container**（`Container::Vertical`、`Container::Horizontal`、`Container::Tab`、`Container::Stacked`）定義**事件與焦點樹**（決定哪個子元件接收鍵盤輸入，以及方向鍵與 `Tab` 如何移動焦點）。
- **`Renderer(container, [&] { ... })`** 定義**視覺版面配置**，同時將所有事件轉發給 `container`。

::: danger 常見陷阱
如果在 `Renderer([&] { ... })` 裡面呼叫了 `child->Render()`，卻**沒有**把該子元件（或包含所有子元件的容器）當作第一個參數傳入，鍵盤與滑鼠事件將**永遠不會**傳遞到這些子元件！
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

請注意，你不需要在 lambda 裡面呼叫 `container->Render()`。分別呼叫每個子元件的 `->Render()`，就能讓你用任何版面配置（`vbox`、`hbox`、`window`、`gridbox` 等）自由排列它們。

## 4. 畫面幀的生命週期

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

- FTXUI 只會在有事件到達或請求動畫幀時才重新繪製。
- 畫面輸出會與前一幀進行**差異比對（diff）**，並包在同步更新（DEC mode 2026）中輸出，以避免閃爍與畫面撕裂。

## 5. 選擇適合的 `App` 模式

| 工廠函式 | 行為 | 適用情境 |
| --- | --- | --- |
| `App::Fullscreen()` | 使用備用螢幕緩衝區（佔滿整個終端機視窗，結束時還原）。 | 全螢幕 TUI（儀表板、編輯器、檔案管理員） |
| `App::TerminalOutput()` | 直接繪製在先前的 shell 輸出下方；支援 `PrintAbove()`。 | 互動式 CLI、提示介面、進度條與日誌工具 |
| `App::FitComponent()` | 自動調整尺寸以貼合元件的最小寬度與高度。 | 精簡的嵌入式元件與快速提示 |
| `App::FixedSize(w, h)` | 固定寬度與高度。 | 測試與固定版面的顯示畫面 |
