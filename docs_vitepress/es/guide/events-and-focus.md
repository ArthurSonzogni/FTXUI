# Eventos, foco y ratón

Cómo enruta FTXUI los eventos de teclado, mueve el foco entre componentes y gestiona la entrada de ratón.

## Cómo recorren los eventos el árbol

Cuando llega una pulsación de tecla, una acción del ratón o un cambio de tamaño:

1. `App` entrega el `Event` al **componente raíz** mediante `root->OnEvent(event)`.
2. Un contenedor (`Container::Vertical`, `Container::Horizontal`, `Container::Tab`, `Container::Stacked`) reenvía primero los eventos de teclado a su **hijo activo actual**.
3. Si el hijo devuelve `true`, el evento se considera **consumido** y se detiene la propagación.
4. Si el hijo devuelve `false`, el contenedor gestiona las teclas de navegación por sí mismo (por ejemplo, `ArrowUp`/`ArrowDown` en `Container::Vertical`, `ArrowLeft`/`ArrowRight` en `Container::Horizontal`, `Tab`/`TabReverse` entre los hijos enfocables).

```text
  App → root->OnEvent(e)
           │
           ▼
     active child->OnEvent(e)
       ├── returns true  → done (event consumed)
       └── returns false → container handles navigation / bubbles up
```

## Interceptar eventos con `CatchEvent`

`CatchEvent` envuelve un componente e inspecciona cada `Event` **antes** de que llegue al componente interno:

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

::: warning Atajos globales frente a `Input`
Como `CatchEvent` se ejecuta **antes** que los hijos, capturar `Event::Character('q')` en la raíz impedirá escribir la letra `'q'` dentro de un cuadro `Input`. Para evitarlo, puedes:
- Usar teclas modificadoras (`Event::CtrlQ`, `Event::Escape`), o
- Comprobar `!input->Focused()` antes de consumir caracteres imprimibles.
:::

### Comprobaciones habituales de `Event`

```cpp
if (event == Event::Return) { /* Enter */ }
if (event == Event::Escape) { /* Esc */ }
if (event == Event::CtrlC)  { /* Ctrl+C */ }
if (event.is_character())   { std::string ch = event.character(); }
if (event.is_mouse())       { Mouse m = event.mouse(); }
```

## Foco y contenedores

- Solo los componentes cuyo método `Focusable()` devuelve `true` pueden recibir foco. Un `Renderer(...)` sin un hijo enfocable no puede recibir foco.
- **`Container::Vertical` / `Container::Horizontal`** mantienen automáticamente el índice del hijo activo (también puedes pasar un puntero explícito `int* selected` para observar o controlar el foco).
- `Container::Tab({tab0, tab1, tab2}, &tab_index)` enruta eventos únicamente a `children[tab_index]`. Las pestañas inactivas no reciben eventos ni toman el foco.
- **Foco programático:** llama a `child->TakeFocus()` para mover el foco directamente a un componente concreto.
- **Consultar el foco:** dentro de un `Renderer`, llama a `child->Focused()` (la hoja exacta tiene el foco) o a `child->Active()` (el hijo o alguno de sus descendientes tiene el foco) para dar estilo a los paneles activos.

```cpp
auto pane = Renderer(child, [&] {
  auto border_style = child->Active() ? borderDouble : borderLight;
  return child->Render() | border_style;
});
```

## Modales y aislamiento de eventos

`Modal(dialog, &show)` superpone `dialog` sobre la interfaz principal (mediante `dbox` + `clear_under`) **y** bloquea todos los eventos de teclado y ratón para que no lleguen al fondo mientras `show == true`:

```cpp
bool show_modal = false;
auto ui = main_component
          | Modal(dialog_component, &show_modal);
```

Cuando `show_modal` pasa a `false`, los eventos vuelven a enrutarse de inmediato a `main_component`.

## Eventos de ratón y detección de clics

El soporte de ratón se activa automáticamente en `App`. Una estructura `Mouse` contiene:

- `mouse.x`, `mouse.y`: coordenadas de celda de la terminal en base 0
- `mouse.button`: `Mouse::Left`, `Mouse::Middle`, `Mouse::Right`, `Mouse::None`, `Mouse::WheelUp`, `Mouse::WheelDown`
- `mouse.motion`: `Mouse::Pressed`, `Mouse::Released`, `Mouse::Moved`
- Modificadores: `mouse.shift`, `mouse.meta`, `mouse.control`

Para detectar clics o el paso del cursor sobre una región personalizada, captura su rectángulo renderizado con `reflect(box)` durante `Render()` y luego comprueba `box.Contain(x, y)` en `OnEvent()`:

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

::: tip Decorador integrado `Hoverable`
Si solo necesitas un booleano para saber si el cursor está encima, usa el decorador integrado en lugar de escribir la detección de ratón a mano:
```cpp
bool is_hovered = false;
auto card = base_component | Hoverable(&is_hovered);
```
:::
