# 事件、焦點與滑鼠

了解 FTXUI 如何路由鍵盤事件、在元件之間切換焦點，以及處理滑鼠輸入。

## 事件如何在元件樹中傳遞

當按鍵、滑鼠操作或視窗縮放事件到達時：

1. `App` 會透過 `root->OnEvent(event)` 將 `Event` 傳遞給**根元件**。
2. 容器（`Container::Vertical`、`Container::Horizontal`、`Container::Tab`、`Container::Stacked`）會優先把鍵盤事件轉發給**目前作用中的子元件**。
3. 若子元件回傳 `true`，表示事件已被**消耗**，傳遞就此停止。
4. 若子元件回傳 `false`，容器會自行處理導覽按鍵（例如 `Container::Vertical` 的 `ArrowUp`/`ArrowDown`、`Container::Horizontal` 的 `ArrowLeft`/`ArrowRight`，以及在可聚焦子元件之間切換的 `Tab`/`TabReverse`）。

```text
  App → root->OnEvent(e)
           │
           ▼
     active child->OnEvent(e)
       ├── returns true  → done (event consumed)
       └── returns false → container handles navigation / bubbles up
```

## 使用 `CatchEvent` 攔截事件

`CatchEvent` 會包裝一個元件，並在每個 `Event` 傳入內部元件**之前**先進行檢查：

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

::: warning 全域快捷鍵與 `Input` 衝突
由於 `CatchEvent` 會在子元件**之前**執行，如果在根元件攔截 `Event::Character('q')`，使用者就無法在 `Input` 輸入框中打出 `'q'`！解決方式有兩種：
- 改用組合鍵（例如 `Event::CtrlQ`、`Event::Escape`），或是
- 在消耗可列印字元之前，先檢查 `!input->Focused()`。
:::

### 常見的 `Event` 檢查方式

```cpp
if (event == Event::Return) { /* Enter */ }
if (event == Event::Escape) { /* Esc */ }
if (event == Event::CtrlC)  { /* Ctrl+C */ }
if (event.is_character())   { std::string ch = event.character(); }
if (event.is_mouse())       { Mouse m = event.mouse(); }
```

## 焦點與容器

- 只有 `Focusable()` 回傳 `true` 的元件才能取得焦點。沒有包含任何可聚焦子元件的 `Renderer(...)` 本身是不可聚焦的。
- **`Container::Vertical` / `Container::Horizontal`** 會自動追蹤作用中子元件的索引（你也可以傳入明確的 `int* selected` 指標來觀察或控制焦點）。
- `Container::Tab({tab0, tab1, tab2}, &tab_index)` 只會把事件路由到 `children[tab_index]`。非作用中的分頁不會接收事件，也不會取得焦點。
- **以程式控制焦點**：呼叫 `child->TakeFocus()` 即可直接將焦點移至特定元件。
- **查詢焦點狀態**：在 `Renderer` 內部呼叫 `child->Focused()`（該葉節點本身擁有焦點）或 `child->Active()`（該元件或其任一子孫擁有焦點），方便為作用中的面板套用不同樣式。

```cpp
auto pane = Renderer(child, [&] {
  auto border_style = child->Active() ? borderDouble : borderLight;
  return child->Render() | border_style;
});
```

## 互動視窗（Modal）與事件隔離

`Modal(dialog, &show)` 會將 `dialog` 疊加在主介面之上（透過 `dbox` 與 `clear_under`），**同時**在 `show == true` 期間阻擋所有鍵盤與滑鼠事件傳遞到背景介面：

```cpp
bool show_modal = false;
auto ui = main_component
          | Modal(dialog_component, &show_modal);
```

當 `show_modal` 變為 `false` 時，事件會立刻恢復路由給 `main_component`。

## 滑鼠事件與點擊偵測

`App` 會自動啟用滑鼠支援。`Mouse` 結構包含以下欄位：

- `mouse.x`、`mouse.y`：以 0 為起點的終端機字元格座標
- `mouse.button`：`Mouse::Left`、`Mouse::Middle`、`Mouse::Right`、`Mouse::None`、`Mouse::WheelUp`、`Mouse::WheelDown`
- `mouse.motion`：`Mouse::Pressed`、`Mouse::Released`、`Mouse::Moved`
- 修飾鍵：`mouse.shift`、`mouse.meta`、`mouse.control`

若要在自訂區域上偵測點擊或游標懸停，請在 `Render()` 期間用 `reflect(box)` 取得渲染後的矩形範圍，再於 `OnEvent()` 裡以 `box.Contain(x, y)` 判斷：

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

::: tip 內建的 `Hoverable`
如果你只需要一個游標懸停的布林值，可以直接使用內建裝飾器，不必手寫滑鼠座標判斷：
```cpp
bool is_hovered = false;
auto card = base_component | Hoverable(&is_hovered);
```
:::
