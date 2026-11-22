# Modelo mental

FTXUI se basa en unas pocas ideas fundamentales. Conocerlas desde el principio hace que la API sea mucho más fácil de usar.

## 1. Tres módulos en capas

FTXUI se divide en tres módulos que se construyen uno sobre otro (`screen → dom → component`):

- **`ftxui/screen`** ([guía](/es/guide/screen), [API](/api/screen)): cuadrícula 2D de bajo nivel compuesta por celdas con estilo (`Screen`, `Surface`, `Cell`, `Color`, `Box`).
- **`ftxui/dom`** ([guía](/es/guide/dom), [API](/api/dom)): árbol de diseño sin estado (`Element`, `hbox`, `vbox`, `border`, `table`, `canvas`, `Render()`).
- **`ftxui/component`** ([guía](/es/guide/component), [API](/api/component)): widgets interactivos y bucle de eventos (`Component`, `Button`, `Input`, `Menu`, `Container`, `App`).

También puedes usar el objetivo paraguas (`ftxui::ftxui` en CMake, `@ftxui//:ftxui` en Bazel, `#include <ftxui/ftxui.hpp>`) para incluir los tres a la vez.

## 2. Elements frente a Components

| | **Element** (`ftxui/dom`) | **Component** (`ftxui/component`) |
| --- | --- | --- |
| **Ciclo de vida** | Efímero, se reconstruye en cada fotograma | Persistente, se crea una vez antes de `app.Loop(...)` |
| **Estado** | Árbol de valores sin estado | Mantiene el estado del foco y referencias a tu estado C++ |
| **Entrada** | No recibe eventos | Maneja `Event` de teclado y ratón mediante `OnEvent` |
| **Ejemplos** | `text`, `hbox`, `vbox`, `border`, `gauge`, `table`, `canvas` | `Button`, `Input`, `Menu`, `Slider`, `Modal`, `Window`, `Container` |

Tu interfaz es una función de tu estado en C++. En lugar de modificar la etiqueta de un widget en el sitio, actualizas variables C++ normales (`std::string`, `int`, `bool`, `std::vector`) y dejas que `Render()` construya un árbol de `Element` nuevo en el siguiente fotograma.

## 3. Árbol de eventos frente a árbol de diseño visual

En FTXUI, el **enrutamiento de eventos y foco** y el **diseño visual** están separados intencionadamente:

- Los **Containers** (`Container::Vertical`, `Container::Horizontal`, `Container::Tab`, `Container::Stacked`) definen el **árbol de eventos y foco** (qué hijo recibe la entrada de teclado y cómo mueven el foco las flechas o `Tab`).
- **`Renderer(container, [&] { ... })`** define el **diseño visual** mientras reenvía todos los eventos a `container`.

::: danger Error común
Llamar a `child->Render()` dentro de `Renderer([&] { ... })` **sin** pasar el hijo (o un contenedor que agrupe a todos los hijos) como primer argumento hará que los eventos de teclado y ratón **nunca** lleguen a esos hijos.
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

Fíjate en que no tienes por qué llamar a `container->Render()` dentro de la lambda. Llamar a `->Render()` en cada hijo te permite organizarlos con el diseño que quieras (`vbox`, `hbox`, `window`, `gridbox`, etc.).

## 4. Ciclo de vida de un fotograma

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

- FTXUI solo redibuja cuando llega un evento o se solicita un fotograma de animación.
- La salida en pantalla se **compara** (diff) contra el fotograma anterior y se envuelve en actualizaciones sincronizadas (modo DEC 2026) para evitar parpadeos y cortes visuales.

## 5. Elegir un modo de `App`

| Factoría | Comportamiento | Ideal para |
| --- | --- | --- |
| `App::Fullscreen()` | Usa el búfer de pantalla alternativo (tamaño completo de la terminal, se restaura al salir). | TUIs a pantalla completa (paneles, editores, gestores de archivos) |
| `App::TerminalOutput()` | Dibuja en línea debajo de la salida anterior del shell; soporta `PrintAbove()`. | CLIs interactivas, prompts, herramientas de progreso y logs |
| `App::FitComponent()` | Ajusta su tamaño al ancho y alto mínimos del componente. | Widgets compactos integrados y prompts rápidos |
| `App::FixedSize(w, h)` | Ancho y alto fijos. | Pruebas y pantallas de diseño fijo |
