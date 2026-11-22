# Cookbook

Common patterns for building applications with FTXUI.

## 1. Background Threads & Live Updates

Never mutate UI state from a background thread directly while `App::Loop()` is rendering. Instead, use `App::Post()` (thread-safe) to run state updates on the main loop thread, and `App::RequestAnimationFrame()` to schedule a redraw:

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

## 2. Dynamic Lists of Components

When a `std::vector` of items grows or shrinks, rebuild the container's children with `DetachAllChildren()` and `Add(...)` (always on the main loop thread, e.g. inside a button callback or `app.Post`):

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

## 3. Scrollable Pane with Mouse Wheel

`vscroll_indicator` is a passive visual indicator. To make a pane user-scrollable with the mouse wheel and keyboard, pair `yframe` + `vscroll_indicator` with `focusPositionRelative` and `CatchEvent`:

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

## 4. Global Shortcuts & Custom `Ctrl+C`

Attach `CatchEvent` at the root component for global shortcuts. Return `true` to consume an event, or `false` to let it propagate to children:

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

## 5. Modals & Confirmation Dialogs

Wrap your main component with `Modal(dialog, &show_modal)`. Add `CatchEvent` on the dialog to dismiss on `Escape`:

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

## 6. Mouse Hit-Testing & Element-Relative Coordinates

Capture an element's rendered bounding `Box` with `| reflect(box)` during `Render()`, then test clicks inside `CatchEvent`:

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

## 7. Streaming CLI Output & Launching External Editors

Use `App::TerminalOutput()` for inline CLIs, `PrintAbove()` to emit permanent scrollback lines, and `WithRestoredIO()` to temporarily hand the terminal back to an external program:

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
