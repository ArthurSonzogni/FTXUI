# Troubleshooting & FAQ

Solutions to the most common build, runtime, and migration issues.

## Events Are Not Reaching My Child Components

**Symptom:** Buttons, inputs, or menus render on screen, but keyboard or mouse input does nothing.

**Cause:** You used `Renderer([&] { return child->Render(); })` without passing `child` (or a `Container` holding your children) as the first argument.

**Fix:** Always pair a `Container` with `Renderer(container, ...)`:
```cpp
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({input->Render(), button->Render()})
         | border;
});
```

See the [Mental Model](/guide/mental-model) guide for details.

---

## Undefined References When Linking Manually

**Symptom:** Linker errors mentioning [`ftxui::Node`](/api/class/node), [`ftxui::Screen`](/api/class/screen), or [`ftxui::Element`](/api/component#element).

**Fix:** With static libraries, link order matters (`component` depends on `dom`, which depends on `screen`):
```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```
With CMake, simply link `ftxui::ftxui` (or `ftxui::component`, which pulls in `dom` and `screen` transitively).

---

## MinGW Cross-Compilation on Linux (`'mutex' is not a member of 'std'`)

**Symptom:** Building with `x86_64-w64-mingw32-g++` on Debian/Ubuntu fails with `error: 'mutex' is not a member of 'std'`.

**Fix:** The default MinGW compiler on Debian/Ubuntu uses the `win32` threading model, which does not implement C++11 `<mutex>`/`<thread>`. Switch to the `posix` variant:
- In your CMake toolchain file, set `CMAKE_CXX_COMPILER` to `x86_64-w64-mingw32-g++-posix`, or
- Run `sudo update-alternatives --config x86_64-w64-mingw32-g++` and select `x86_64-w64-mingw32-g++-posix`.

---

## Windows Macro Collisions (`RGB`, `DrawText`, `min`, `max`)

**Symptom:** Strange compile errors in `<ftxui/screen/color.hpp>` or `<ftxui/dom/canvas.hpp>` after including `<windows.h>`.

**Fix:** Define `NOMINMAX` and `WIN32_LEAN_AND_MEAN` before including `<windows.h>` (or pass `-DNOMINMAX` in CMake). FTXUI undefines `RGB` and `DrawText` in its headers, but defining `NOMINMAX` prevents `std::min`/`std::max` conflicts.

---

## Colors, Unicode, or Mouse Not Working in a Terminal

- **Override color detection:**
  ```cpp
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  ```
- **Disable Unicode block characters / use ASCII fallbacks:**
  ```cpp
  auto quirks = Terminal::GetQuirks();
  quirks.SetComponentAscii(true);
  Terminal::SetQuirks(quirks);
  ```
- **Windows 7 / legacy consoles:** Legacy Windows `conhost` (before Windows 10) does not support Virtual Terminal escape sequences (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`). Use Windows Terminal or a modern VT100-compatible terminal.

---

## Migrating from FTXUI v6 to v7

FTXUI v7 renamed three core classes for clarity (deprecated aliases remain available for backwards compatibility):

| FTXUI v6 (deprecated) | FTXUI v7 | Header |
| --- | --- | --- |
| `ScreenInteractive` | `App` | `<ftxui/component/app.hpp>` |
| `Pixel` / `PixelAt()` | `Cell` / `CellAt()` | `<ftxui/screen/cell.hpp>` |
| `Image` / `DrawImage()` | `Surface` / `DrawSurface()` | `<ftxui/screen/surface.hpp>` |
