# Solución de problemas y preguntas frecuentes

Soluciones a los problemas más habituales de compilación, ejecución y migración.

## Los eventos no llegan a mis componentes hijos

**Síntoma:** los botones, entradas o menús se renderizan en pantalla, pero el teclado y el ratón no hacen nada.

**Causa:** has usado `Renderer([&] { return child->Render(); })` sin pasar `child` (o un `Container` que contenga a tus hijos) como primer argumento.

**Solución:** combina siempre un `Container` con `Renderer(container, ...)`:
```cpp
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({input->Render(), button->Render()})
         | border;
});
```

Consulta la guía de [Modelo mental](/es/guide/mental-model) para más detalles.

---

## Referencias indefinidas al enlazar manualmente

**Síntoma:** errores del enlazador que mencionan [`ftxui::Node`](/api/class/node), [`ftxui::Screen`](/api/class/screen) o [`ftxui::Element`](/api/component#element).

**Solución:** con bibliotecas estáticas, el orden de enlazado importa (`component` depende de `dom`, que a su vez depende de `screen`):
```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```
Con CMake, simplemente enlaza `ftxui::ftxui` (o `ftxui::component`, que arrastra `dom` y `screen` de forma transitiva).

---

## Compilación cruzada con MinGW en Linux (`'mutex' is not a member of 'std'`)

**Síntoma:** compilar con `x86_64-w64-mingw32-g++` en Debian/Ubuntu falla con `error: 'mutex' is not a member of 'std'`.

**Solución:** el compilador MinGW por defecto en Debian/Ubuntu usa el modelo de hilos `win32`, que no implementa `<mutex>` ni `<thread>` de C++11. Cambia a la variante `posix`:
- En tu archivo toolchain de CMake, establece `CMAKE_CXX_COMPILER` a `x86_64-w64-mingw32-g++-posix`, o
- Ejecuta `sudo update-alternatives --config x86_64-w64-mingw32-g++` y selecciona `x86_64-w64-mingw32-g++-posix`.

---

## Colisiones de macros en Windows (`RGB`, `DrawText`, `min`, `max`)

**Síntoma:** errores de compilación extraños en `<ftxui/screen/color.hpp>` o `<ftxui/dom/canvas.hpp>` después de incluir `<windows.h>`.

**Solución:** define `NOMINMAX` y `WIN32_LEAN_AND_MEAN` antes de incluir `<windows.h>` (o pasa `-DNOMINMAX` en CMake). FTXUI elimina la definición de `RGB` y `DrawText` en sus cabeceras, pero definir `NOMINMAX` evita conflictos con `std::min` y `std::max`.

---

## Los colores, Unicode o el ratón no funcionan en una terminal

- **Forzar la detección de color:**
  ```cpp
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  ```
- **Desactivar caracteres de bloque Unicode / usar alternativas ASCII:**
  ```cpp
  auto quirks = Terminal::GetQuirks();
  quirks.SetComponentAscii(true);
  Terminal::SetQuirks(quirks);
  ```
- **Windows 7 / consolas heredadas:** el antiguo `conhost` de Windows (anterior a Windows 10) no soporta secuencias de escape de terminal virtual (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`). Usa Windows Terminal o una terminal moderna compatible con VT100.

---

## Migrar de FTXUI v6 a v7

FTXUI v7 renombró tres clases principales por claridad (los alias obsoletos siguen disponibles por retrocompatibilidad):

| FTXUI v6 (obsoleto) | FTXUI v7 | Cabecera |
| --- | --- | --- |
| `ScreenInteractive` | `App` | `<ftxui/component/app.hpp>` |
| `Pixel` / `PixelAt()` | `Cell` / `CellAt()` | `<ftxui/screen/cell.hpp>` |
| `Image` / `DrawImage()` | `Surface` / `DrawSurface()` | `<ftxui/screen/surface.hpp>` |
