# Mental Model

FTXUI is built around a few core ideas. Knowing them upfront makes the API much easier to use.

## 1. Three Layered Modules

FTXUI is split into three modules that build on each other (`screen → dom → component`):

- **`ftxui/screen`** ([guide](/guide/screen), [API](/api/screen)): low-level 2D grid of styled cells (`Screen`, `Surface`, `Cell`, `Color`, `Box`).
- **`ftxui/dom`** ([guide](/guide/dom), [API](/api/dom)): stateless layout tree (`Element`, `hbox`, `vbox`, `border`, `table`, `canvas`, `Render()`).
- **`ftxui/component`** ([guide](/guide/component), [API](/api/component)): interactive widgets and event loop (`Component`, `Button`, `Input`, `Menu`, `Container`, `App`).

You can also use the umbrella target (`ftxui::ftxui` in CMake, `@ftxui//:ftxui` in Bazel, `#include <ftxui/ftxui.hpp>`) to include all three at once.

## 2. Elements vs. Components

| | **Element** (`ftxui/dom`) | **Component** (`ftxui/component`) |
| --- | --- | --- |
| **Lifetime** | Ephemeral, rebuilt every frame | Persistent, created once before `app.Loop(...)` |
| **State** | Stateless value tree | Holds focus state and references to your C++ state |
| **Input** | Does not receive events | Handles keyboard and mouse `Event`s via `OnEvent` |
| **Examples** | `text`, `hbox`, `vbox`, `border`, `gauge`, `table`, `canvas` | `Button`, `Input`, `Menu`, `Slider`, `Modal`, `Window`, `Container` |

Your UI is a function of your C++ state. Instead of mutating a widget's label in place, you update plain C++ variables (`std::string`, `int`, `bool`, `std::vector`) and let `Render()` build a fresh `Element` tree on the next frame.

## 3. Event Tree vs. Visual Layout Tree

In FTXUI, **event/focus routing** and **visual layout** are intentionally separate:

- **Containers** (`Container::Vertical`, `Container::Horizontal`, `Container::Tab`, `Container::Stacked`) define the **event & focus tree** (which child receives keyboard input, and how arrow keys / `Tab` move focus).
- **`Renderer(container, [&] { ... })`** defines the **visual layout** while forwarding all events to `container`.

::: danger Common Pitfall
Calling `child->Render()` inside `Renderer([&] { ... })` **without** passing the child (or a container holding all children) as the first argument means keyboard and mouse events will **never** reach those children!
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

Notice that you do not have to call `container->Render()` inside the lambda. Calling each child's `->Render()` lets you arrange them in any layout (`vbox`, `hbox`, `window`, `gridbox`, etc.).

## 4. The Frame Lifecycle

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

- FTXUI only redraws when an event arrives or an animation frame is requested.
- Screen output is **diffed** against the previous frame and wrapped in synchronized updates (DEC mode 2026) to prevent flicker and tearing.

## 5. Choosing an `App` Mode

| Factory | Behavior | Best for |
| --- | --- | --- |
| `App::Fullscreen()` | Uses the alternate screen buffer (full terminal size, restored on exit). | Full-screen TUIs (dashboards, editors, file managers) |
| `App::TerminalOutput()` | Draws inline below previous shell output; supports `PrintAbove()`. | Interactive CLIs, prompts, progress/log tools |
| `App::FitComponent()` | Sizes itself to the component's minimum width and height. | Compact embedded widgets and quick prompts |
| `App::FixedSize(w, h)` | Fixed width and height. | Tests and fixed-layout displays |
