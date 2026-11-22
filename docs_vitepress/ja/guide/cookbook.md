# クックブック

FTXUI でアプリケーションを構築する際によく使う実践パターン集です。

## 1. バックグラウンドスレッドとリアルタイム更新

`App::Loop()` が描画を行っている間、バックグラウンドスレッドから UI の状態を直接書き換えてはいけません。代わりに、スレッドセーフな `App::Post()` を使ってメインループのスレッド上で状態を更新し、`App::RequestAnimationFrame()` で再描画をスケジュールします。

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

## 2. コンポーネントの動的リスト

アイテムの `std::vector` が増減するときは、`DetachAllChildren()` と `Add(...)` を使ってコンテナの子要素を再構築します。この操作は必ずメインループスレッド上（ボタンのコールバックや `app.Post` の中など）で行ってください。

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

## 3. マウスホイール対応のスクロールペイン

`vscroll_indicator` は受動的な視覚インジケータにすぎません。マウスホイールやキーボードでユーザーがスクロールできるペインを作るには、`yframe` + `vscroll_indicator` に `focusPositionRelative` と `CatchEvent` を組み合わせます。

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

## 4. グローバルショートカットとカスタム `Ctrl+C`

グローバルなキーボードショートカットを実装するには、ルートコンポーネントに `CatchEvent` を適用します。イベントを消費する場合は `true`、子コンポーネントへ伝播させる場合は `false` を返します。

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

## 5. モーダルと確認ダイアログ

メインコンポーネントを `Modal(dialog, &show_modal)` でラップします。`Escape` キーで閉じるようにするには、ダイアログ側に `CatchEvent` を追加します。

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

## 6. マウスのヒットテストと要素相対座標

`Render()` の中で `| reflect(box)` を使って要素の描画領域（`Box`）を取得し、`CatchEvent` 内でクリック座標を判定します。

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

## 7. CLI のストリーミング出力と外部エディタの起動

インライン CLI には `App::TerminalOutput()` を使い、`PrintAbove()` でスクロールバックに残るログ行を出力できます。また、`WithRestoredIO()` を使うと、一時的に端末の制御を外部プログラムへ明け渡すことができます。

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
