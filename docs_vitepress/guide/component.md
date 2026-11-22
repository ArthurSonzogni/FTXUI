# Component Module (`ftxui/component`)

The **Component** module builds interactive, stateful terminal applications on top of `ftxui/dom` and `ftxui/screen`.

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
```

<FtxuiDemo file="component/homescreen" height="520px" title="Live demo: component/homescreen (click tabs or use arrow keys)" />

## Built-in Interactive Widgets

All built-in components bind directly to your plain C++ state via pointers or `Ref<T>`:

| Component | Quick Example | Live Demo |
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

Every widget also accepts an `*Option` struct (e.g., `ButtonOption::Animated()`, `MenuOption::HorizontalAnimated()`, `InputOption`) for styling, animations, and callbacks (`on_change`, `on_enter`, `transform`).

## Composing Layout & Events (`Container` + `Renderer`)

In FTXUI, **containers** manage keyboard/mouse routing and focus, while **`Renderer`** controls visual layout:

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

### Container Types

- `Container::Vertical({...})`: Up/Down arrows and Tab move focus vertically.
- `Container::Horizontal({...})`: Left/Right arrows and Tab move focus horizontally.
- `Container::Tab({...}, &active_tab)`: only the active child renders and receives events.
- `Container::Stacked({...})`: renders all children in order (useful for floating `Window` panes).

## Component Decorators

Components compose with the same `|` operator as DOM elements:

- `CatchEvent(fn)`: intercept events before the child sees them (return `true` to consume, `false` to bubble). See [Events, Focus & Mouse](/guide/events-and-focus).
- `Maybe(&show)`: conditionally show/hide a component and disable its event handling.
- `Modal(dialog, &show)`: overlay `dialog` on top of the main UI and block background events while `show` is `true`. See [`component/modal_dialog`](/examples/component/modal_dialog).
- `Hoverable(&is_hovered)`: track whether the mouse cursor is hovering over a component.

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

## Updating From Background Threads

`App` only redraws when an event arrives or a frame is requested. To update UI state safely from a worker thread, use `app.Post(...)` (which runs on the main loop thread) followed by `app.RequestAnimationFrame()`:

```cpp
std::thread worker([&] {
  auto result = fetch_data();
  app.Post([&, result = std::move(result)] {
    data = result;
  });
  app.RequestAnimationFrame();
});
```

## Printing Above an Inline App (`PrintAbove`)

In `App::TerminalOutput()` mode, the interactive UI renders inline below previous shell output. Use `app.PrintAbove(element)` (thread-safe) to append permanent log or chat lines above the live prompt:

```cpp
auto app = App::TerminalOutput();
app.PrintAbove(paragraph("Build finished!") | color(Color::Green));
```

See [`component/print_above`](/examples/component/print_above).

## Custom Event Loops (`Loop`)

If you want to integrate FTXUI into an existing game loop or network reactor, use [`ftxui::Loop`](/api/class/loop) instead of `app.Loop(...)`:

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

See [`component/custom_loop`](/examples/component/custom_loop).

## Restoring Terminal I/O (`WithRestoredIO`)

To temporarily suspend raw terminal mode and launch an external program (such as `$EDITOR`, `less`, or `git`), wrap the call with `app.WithRestoredIO(...)`:

```cpp
auto edit_btn = Button("Open Editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
```

See [`component/with_restored_io`](/examples/component/with_restored_io).

## Next Steps

- [Events, Focus & Mouse](/guide/events-and-focus): event routing, focus, and mouse hit-testing
- [Custom Widgets](/guide/custom-widgets): subclassing `ComponentBase` and `Node`
- [`ftxui/component` API Reference](/api/component): full list of functions and options
