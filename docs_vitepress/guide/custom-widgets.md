# Custom Widgets (`ComponentBase` & `Node`)

Most FTXUI apps can be built by composing built-in components with `Renderer`, `Container`, and `CatchEvent`. When you need a reusable, self-contained widget or custom low-level cell drawing, you can subclass `ComponentBase` or `Node` directly.

## Custom Component (`ComponentBase`)

Subclass [`ftxui::ComponentBase`](/api/class/componentbase) when you want a stateful interactive widget with its own rendering, event handling, and focus behavior.

### Key Virtual Methods

| Method | Purpose |
| --- | --- |
| `Element OnRender()` | Return the `Element` tree for this frame (called every redraw). |
| `bool OnEvent(Event event)` | Handle keyboard/mouse input. Return `true` if consumed, `false` to let parents handle it. |
| `bool Focusable() const` | Return `true` if this widget can receive keyboard focus via `Tab` / arrow keys. |
| `void OnAnimation(animation::Params& params)` | Advance animations; call `animation::RequestAnimationFrame()` while still animating. |

### Example: A Reusable Counter Widget

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
Always construct components with `ftxui::Make<MyWidget>(args...)`, which returns a `Component` (`std::shared_ptr<ComponentBase>`).
:::

### Composing Child Components Inside a Custom Component

If your custom component owns child components (e.g., an `Input` and a `Button`), register them with `Add(child)` in the constructor so `ComponentBase` wires up parent pointers and default event/focus forwarding:

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

## Custom DOM Element (`Node`)

Subclass [`ftxui::Node`](/api/class/node) when you need direct per-cell rendering onto a `Screen` (for example, a custom chart, hex viewer, or pixel shader).

### The Three Layout Passes

Every frame, [`ftxui::Render(screen, root)`](/api/dom#render) runs three passes over the `Node` tree:

1. `ComputeRequirement()` (bottom-up): set `requirement_.min_x`, `requirement_.min_y`, `requirement_.flex_grow_x`, `requirement_.flex_grow_y`, etc.
2. `SetBox(Box box)` (top-down): receive the allocated bounding box (`box_`) and assign sub-boxes to any children.
3. `Render(Screen& screen)`: draw directly into `screen.CellAt(x, y)` within `box_`.

### Example: A Custom Checkerboard Node

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

::: tip Performance: `screen.stencil` and `FastCellAt`
`screen.CellAt(x, y)` is already bounds- and stencil-checked (writes outside `screen.stencil` are safely ignored). For very large nodes inside a `frame`, you can skip offscreen work by iterating over `Box::Intersection(box_, screen.stencil)` and calling `screen.FastCellAt(x, y)`.
:::
