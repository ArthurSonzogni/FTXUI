# Events, Focus & Mouse

How FTXUI routes keyboard events, moves focus between components, and handles mouse input.

## How Events Travel Through the Tree

When a key press, mouse action, or resize arrives:

1. `App` delivers the `Event` to the **root component** via `root->OnEvent(event)`.
2. A container (`Container::Vertical`, `Container::Horizontal`, `Container::Tab`, `Container::Stacked`) forwards keyboard events to its **currently active child** first.
3. If the child returns `true`, the event is **consumed** and propagation stops.
4. If the child returns `false`, the container handles navigation keys itself (e.g., `ArrowUp`/`ArrowDown` in `Container::Vertical`, `ArrowLeft`/`ArrowRight` in `Container::Horizontal`, `Tab`/`TabReverse` across focusable children).

```text
  App → root->OnEvent(e)
           │
           ▼
     active child->OnEvent(e)
       ├── returns true  → done (event consumed)
       └── returns false → container handles navigation / bubbles up
```

## Intercepting Events with `CatchEvent`

`CatchEvent` wraps a component and inspects every `Event` **before** it reaches the inner component:

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

::: warning Global Shortcuts vs. `Input`
Because `CatchEvent` runs **before** children, catching `Event::Character('q')` at the root will prevent typing `'q'` into an `Input` box! Either:
- Use modifier keys (`Event::CtrlQ`, `Event::Escape`), or
- Check `!input->Focused()` before consuming printable characters.
:::

### Common `Event` Checks

```cpp
if (event == Event::Return) { /* Enter */ }
if (event == Event::Escape) { /* Esc */ }
if (event == Event::CtrlC)  { /* Ctrl+C */ }
if (event.is_character())   { std::string ch = event.character(); }
if (event.is_mouse())       { Mouse m = event.mouse(); }
```

## Focus & Containers

- Only components where `Focusable()` returns `true` can receive focus. `Renderer(...)` without a focusable child is not focusable.
- **`Container::Vertical` / `Container::Horizontal`** automatically track the active child index (you can also pass an explicit `int* selected` pointer to observe or control focus).
- `Container::Tab({tab0, tab1, tab2}, &tab_index)` routes events only to `children[tab_index]`. Inactive tabs do not receive events or take focus.
- **Programmatic focus:** Call `child->TakeFocus()` to move focus directly to a specific component.
- **Querying focus:** Inside a `Renderer`, call `child->Focused()` (exact leaf has focus) or `child->Active()` (child or one of its descendants has focus) to style active panes.

```cpp
auto pane = Renderer(child, [&] {
  auto border_style = child->Active() ? borderDouble : borderLight;
  return child->Render() | border_style;
});
```

## Modals & Event Isolation

`Modal(dialog, &show)` layers `dialog` on top of the main UI (via `dbox` + `clear_under`) **and** blocks all keyboard/mouse events from reaching the background while `show == true`:

```cpp
bool show_modal = false;
auto ui = main_component
          | Modal(dialog_component, &show_modal);
```

When `show_modal` becomes `false`, events immediately resume routing to `main_component`.

## Mouse Events & Hit-Testing

Mouse support is enabled automatically in `App`. A `Mouse` struct contains:

- `mouse.x`, `mouse.y`: 0-based terminal cell coordinates
- `mouse.button`: `Mouse::Left`, `Mouse::Middle`, `Mouse::Right`, `Mouse::None`, `Mouse::WheelUp`, `Mouse::WheelDown`
- `mouse.motion`: `Mouse::Pressed`, `Mouse::Released`, `Mouse::Moved`
- Modifiers: `mouse.shift`, `mouse.meta`, `mouse.control`

To detect clicks or hover on a custom region, capture its rendered rectangle with `reflect(box)` during `Render()`, then test `box.Contain(x, y)` in `OnEvent()`:

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

::: tip Built-in `Hoverable`
If you only need a hover boolean, use the built-in decorator instead of writing mouse hit-testing manually:
```cpp
bool is_hovered = false;
auto card = base_component | Hoverable(&is_hovered);
```
:::
