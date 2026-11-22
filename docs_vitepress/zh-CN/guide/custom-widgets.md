# 自定义控件 (`ComponentBase` 与 `Node`)

大多数 FTXUI 应用只需组合内置组件与 `Renderer`、`Container`、`CatchEvent` 即可完成。当你需要封装可复用的独立控件，或进行底层逐单元格绘制时，可以直接继承 `ComponentBase` 或 `Node`。

## 自定义组件 (`ComponentBase`)

当你需要一个带状态、具备独立渲染、事件处理和焦点行为的交互式控件时，继承 [`ftxui::ComponentBase`](/api/class/componentbase)。

### 关键虚函数

| 方法 | 作用 |
| --- | --- |
| `Element OnRender()` | 返回当前帧的 `Element` 树（每次重绘时调用）。 |
| `bool OnEvent(Event event)` | 处理键盘/鼠标输入。消费事件返回 `true`，交由父组件处理返回 `false`。 |
| `bool Focusable() const` | 返回 `true` 表示该控件可以通过 `Tab` 或方向键获取键盘焦点。 |
| `void OnAnimation(animation::Params& params)` | 推进动画状态；动画尚未结束时需调用 `animation::RequestAnimationFrame()`。 |

### 示例：可复用的计数器控件

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
请始终使用 `ftxui::Make<MyWidget>(args...)` 构造组件，它会返回一个 `Component`（即 `std::shared_ptr<ComponentBase>`）。
:::

### 在自定义组件中组合子组件

如果自定义组件内部持有其他子组件（例如一个 `Input` 和一个 `Button`），请在构造函数中调用 `Add(child)` 进行注册，这样 `ComponentBase` 就能自动接好父指针以及默认的事件和焦点转发：

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

## 自定义 DOM 元素 (`Node`)

当你需要直接向 `Screen` 逐单元格绘制内容时（例如自定义图表、十六进制查看器或像素着色器），可以继承 [`ftxui::Node`](/api/class/node)。

### 三个布局阶段

每一帧 [`ftxui::Render(screen, root)`](/api/dom#render) 都会对 `Node` 树执行三遍遍历：

1. `ComputeRequirement()`（自底向上）：设置 `requirement_.min_x`、`requirement_.min_y`、`requirement_.flex_grow_x`、`requirement_.flex_grow_y` 等尺寸需求。
2. `SetBox(Box box)`（自顶向下）：接收分配到的边界框（`box_`），并为子节点分配各自的子区域。
3. `Render(Screen& screen)`：在 `box_` 范围内直接向 `screen.CellAt(x, y)` 绘制内容。

### 示例：自定义棋盘格节点

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

::: tip 性能优化：`screen.stencil` 与 `FastCellAt`
`screen.CellAt(x, y)` 内置了边界与裁剪区检查（超出 `screen.stencil` 的写入会被安全忽略）。对于位于 `frame` 内部的超大节点，可以先用 `Box::Intersection(box_, screen.stencil)` 求出可见区域，再配合 `screen.FastCellAt(x, y)` 跳过屏幕外的无效绘制。
:::
