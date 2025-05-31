# Modules {#modules}

![title-img](https://nsm09.casimages.com/img/2025/05/30//2505300816063242518595251.jpg)

FTXUI is organized into three modules, each building upon the previous:

1. @subpage module-screen — low-level rendering
2. @subpage module-dom — layout and composition
3. @subpage module-component — user interaction

---

[ @subpage module-screen ]

Defines:

- **`ftxui::Screen`**: a 2D grid of styled characters.
- **`ftxui::Pixel`**: the unit of rendering.
- Helpers like `ftxui::Color` and `Dimension`.

Use for direct terminal drawing and styling.

---

[ @subpage module-dom ]

Provides:

- **`ftxui::Element`**: a tree structure for layout and UI.
- Composable and responsive elements.
- `Render()` to draw onto a `Screen`.

Ideal for structured, styled UIs.

---

[ @subpage module-component ]

Adds:

- **`ftxui::Component`**: stateful, interactive widgets.
- Built-ins: `Checkbox`, `Input`, `Menu`, `Button`.
- Supports keyboard/cursor input and composition.

Use for interactive apps.

---

Modules can be used independently, or together: `screen → dom → component`.
