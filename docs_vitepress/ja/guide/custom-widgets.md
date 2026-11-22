# カスタムウィジェット (`ComponentBase` & `Node`)

ほとんどの FTXUI アプリケーションは、組み込みコンポーネントを `Renderer`、`Container`、`CatchEvent` で組み合わせるだけで構築できます。再利用可能で自己完結したウィジェットや、セル単位の独自描画が必要な場合は、`ComponentBase` や `Node` を直接継承して実装します。

## カスタムコンポーネント (`ComponentBase`)

独自の描画・イベント処理・フォーカス動作を持つ状態付きインタラクティブウィジェットを作る場合は、[`ftxui::ComponentBase`](/api/class/componentbase) を継承します。

### 主な仮想メソッド

| メソッド | 役割 |
| --- | --- |
| `Element OnRender()` | このフレームの `Element` ツリーを返す（再描画ごとに呼ばれる）。 |
| `bool OnEvent(Event event)` | キーボード・マウス入力を処理する。消費したら `true`、親に委ねるなら `false` を返す。 |
| `bool Focusable() const` | `Tab` や矢印キーでキーボードフォーカスを受け取れる場合は `true` を返す。 |
| `void OnAnimation(animation::Params& params)` | アニメーションを進める。継続中は `animation::RequestAnimationFrame()` を呼ぶ。 |

### 例: 再利用可能なカウンターウィジェット

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
コンポーネントの生成には常に `ftxui::Make<MyWidget>(args...)` を使用してください。戻り値は `Component`（`std::shared_ptr<ComponentBase>`）です。
:::

### カスタムコンポーネント内で子コンポーネントを組み合わせる

カスタムコンポーネントが子コンポーネント（例: `Input` と `Button`）を所有する場合は、コンストラクタ内で `Add(child)` を使って登録してください。これにより、`ComponentBase` が親ポインタの設定とデフォルトのイベント・フォーカス転送を自動的に配線してくれます。

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

## カスタム DOM 要素 (`Node`)

`Screen` に対してセル単位の直接描画を行いたい場合（カスタムチャート、バイナリエディタ、ピクセルシェーダなど）は、[`ftxui::Node`](/api/class/node) を継承します。

### 3つのレイアウトパス

毎フレーム、[`ftxui::Render(screen, root)`](/api/dom#render) は `Node` ツリーに対して以下の3つのパスを実行します。

1. `ComputeRequirement()`（ボトムアップ）: `requirement_.min_x`、`requirement_.min_y`、`requirement_.flex_grow_x`、`requirement_.flex_grow_y` などを設定します。
2. `SetBox(Box box)`（トップダウン）: 割り当てられたバウンディングボックス（`box_`）を受け取り、子ノードがあればサブボックスを割り当てます。
3. `Render(Screen& screen)`: `box_` の範囲内で `screen.CellAt(x, y)` に直接描画します。

### 例: カスタム市松模様ノード

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

::: tip パフォーマンス: `screen.stencil` と `FastCellAt`
`screen.CellAt(x, y)` は境界チェックとステンシルチェックを内部で行います（`screen.stencil` の外への書き込みは安全に無視されます）。`frame` 内に非常に大きなノードを配置する場合は、`Box::Intersection(box_, screen.stencil)` の範囲だけを走査し、`screen.FastCellAt(x, y)` を呼ぶことで画面外の描画処理をスキップできます。
:::
