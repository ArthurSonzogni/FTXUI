# 自訂元件（`ComponentBase` 與 `Node`）

大多數 FTXUI 應用程式都能透過組合內建元件搭配 `Renderer`、`Container` 與 `CatchEvent` 完成。當你需要可重複使用的獨立元件，或是自訂底層的逐格繪製時，可以直接繼承 `ComponentBase` 或 `Node`。

## 自訂 Component（`ComponentBase`）

當你想打造一個具備自身渲染邏輯、事件處理與焦點行為的有狀態互動元件時，請繼承 [`ftxui::ComponentBase`](/api/class/componentbase)。

### 關鍵虛擬函式

| 方法 | 用途 |
| --- | --- |
| `Element OnRender()` | 回傳本幀的 `Element` 樹（每次重繪時呼叫）。 |
| `bool OnEvent(Event event)` | 處理鍵盤與滑鼠輸入。已消耗請回傳 `true`，若要交給父元件處理則回傳 `false`。 |
| `bool Focusable() const` | 若此元件可透過 `Tab` 或方向鍵接收鍵盤焦點，回傳 `true`。 |
| `void OnAnimation(animation::Params& params)` | 推進動畫狀態；動畫仍在進行時需呼叫 `animation::RequestAnimationFrame()`。 |

### 範例：可重複使用的計數器元件

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

using namespace ftxui;

class CounterWidget : public ComponentBase {
 public:
  explicit CounterWidget(int* value) : value_(value) {}

  Element OnRender() override {
    auto label = text(" Count: " + std::to_string(*value_) + " ");
    auto hint = text(" (+/- or click) ") | dim;
    auto body = hbox({label, hint});

    if (Focused()) {
      body = body | inverted;
    }

    return body
           | borderRounded
           | reflect(box_);
  }

  bool OnEvent(Event event) override {
    if (event == Event::Character('+') || event == Event::ArrowRight) {
      (*value_)++;
      return true;
    }
    if (event == Event::Character('-') || event == Event::ArrowLeft) {
      (*value_)--;
      return true;
    }
    if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y)) {
      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Pressed) {
        TakeFocus();
        (*value_)++;
        return true;
      }
      if (event.mouse().button == Mouse::WheelUp) {
        (*value_)++;
        return true;
      }
      if (event.mouse().button == Mouse::WheelDown) {
        (*value_)--;
        return true;
      }
    }
    return false;
  }

  bool Focusable() const override { return true; }

 private:
  int* value_;
  Box box_;
};

// Factory function matching FTXUI's convention:
Component Counter(int* value) {
  return Make<CounterWidget>(value);
}
```

::: tip `Make<T>(...)`
建立元件時請一律使用 `ftxui::Make<MyWidget>(args...)`，它會回傳 `Component`（即 `std::shared_ptr<ComponentBase>`）。
:::

### 在自訂元件中組合子元件

如果你的自訂元件內部包含子元件（例如一個 `Input` 和一個 `Button`），請在建構子中使用 `Add(child)` 註冊它們，讓 `ComponentBase` 自動接好父節點指標，以及預設的事件與焦點轉發：

```cpp
class SearchBar : public ComponentBase {
 public:
  SearchBar(std::string* query, std::function<void()> on_submit) {
    input_ = Input(query, "Search...");
    submit_ = Button("Go", std::move(on_submit));
    Add(Container::Horizontal({input_, submit_}));
  }

  Element OnRender() override {
    auto input_el = input_->Render() | flex;
    auto submit_el = submit_->Render();
    return hbox({input_el, submit_el}) | border;
  }

 private:
  Component input_;
  Component submit_;
};
```

## 自訂 DOM 元素（`Node`）

當你需要直接在 `Screen` 上逐格繪製內容時（例如自訂圖表、十六進位檢視器或像素著色器），可以繼承 [`ftxui::Node`](/api/class/node)。

### 三階段版面配置流程

每一幀，[`ftxui::Render(screen, root)`](/api/dom#render) 都會對 `Node` 樹執行三次走訪：

1. `ComputeRequirement()`（由下而上）：設定 `requirement_.min_x`、`requirement_.min_y`、`requirement_.flex_grow_x`、`requirement_.flex_grow_y` 等尺寸需求。
2. `SetBox(Box box)`（由上而下）：接收分配到的邊界框（`box_`），並將子區域分配給各個子節點。
3. `Render(Screen& screen)`：在 `box_` 範圍內直接繪製到 `screen.CellAt(x, y)`。

### 範例：自訂棋盤格節點

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>

using namespace ftxui;

class CheckerboardNode : public Node {
 public:
  CheckerboardNode(Color a, Color b) : a_(a), b_(b) {}

  void ComputeRequirement() override {
    requirement_.min_x = 4;
    requirement_.min_y = 2;
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 1;
  }

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        auto& cell = screen.CellAt(x, y);
        cell.character = " ";
        cell.background_color = ((x + y) % 2 == 0) ? a_ : b_;
      }
    }
  }

 private:
  Color a_;
  Color b_;
};

Element checkerboard(Color a, Color b) {
  return std::make_shared<CheckerboardNode>(a, b);
}
```

::: tip 效能提示：`screen.stencil` 與 `FastCellAt`
`screen.CellAt(x, y)` 本身已經包含邊界與裁切區（stencil）檢查，超出 `screen.stencil` 的寫入會被安全忽略。如果節點位於 `frame` 內部且尺寸非常龐大，可以改為只走訪 `Box::Intersection(box_, screen.stencil)` 並呼叫 `screen.FastCellAt(x, y)`，以略過畫面外的無效計算。
:::
