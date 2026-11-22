# Módulo Component (`ftxui/component`)

El módulo **Component** permite crear aplicaciones de terminal interactivas y con estado sobre `ftxui/dom` y `ftxui/screen`.

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
```

<FtxuiDemo file="component/homescreen" height="520px" title="Live demo: component/homescreen (click tabs or use arrow keys)" />

## Widgets interactivos integrados

Todos los componentes integrados se enlazan directamente con tu estado C++ normal mediante punteros o `Ref<T>`:

| Componente | Ejemplo rápido | Demo en vivo |
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

Cada widget acepta también una estructura `*Option` (por ejemplo, `ButtonOption::Animated()`, `MenuOption::HorizontalAnimated()`, `InputOption`) para personalizar estilos, animaciones y callbacks (`on_change`, `on_enter`, `transform`).

## Componer diseño y eventos (`Container` + `Renderer`)

En FTXUI, los **contenedores** gestionan el enrutamiento de teclado y ratón así como el foco, mientras que **`Renderer`** controla el diseño visual:

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

### Tipos de contenedores

- `Container::Vertical({...})`: las flechas arriba/abajo y Tab mueven el foco verticalmente.
- `Container::Horizontal({...})`: las flechas izquierda/derecha y Tab mueven el foco horizontalmente.
- `Container::Tab({...}, &active_tab)`: solo el hijo activo se renderiza y recibe eventos.
- `Container::Stacked({...})`: renderiza todos los hijos en orden (útil para paneles `Window` flotantes).

## Decoradores de componentes

Los componentes se componen con el mismo operador `|` que los elementos del DOM:

- `CatchEvent(fn)`: intercepta los eventos antes de que los vea el hijo (devuelve `true` para consumirlos o `false` para dejarlos propagar). Consulta [Eventos, foco y ratón](/es/guide/events-and-focus).
- `Maybe(&show)`: muestra u oculta condicionalmente un componente y desactiva su gestión de eventos.
- `Modal(dialog, &show)`: superpone `dialog` sobre la interfaz principal y bloquea los eventos del fondo mientras `show` sea `true`. Consulta [`component/modal_dialog`](/examples/component/modal_dialog).
- `Hoverable(&is_hovered)`: detecta si el cursor del ratón está encima de un componente.

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

## Actualizar desde hilos en segundo plano

`App` solo redibuja cuando llega un evento o se solicita un fotograma. Para actualizar el estado de la interfaz de forma segura desde un hilo trabajador, usa `app.Post(...)` (que se ejecuta en el hilo del bucle principal) seguido de `app.RequestAnimationFrame()`:

```cpp
std::thread worker([&] {
  auto result = fetch_data();
  app.Post([&, result = std::move(result)] {
    data = result;
  });
  app.RequestAnimationFrame();
});
```

## Imprimir encima de una aplicación en línea (`PrintAbove`)

En el modo `App::TerminalOutput()`, la interfaz interactiva se renderiza en línea debajo de la salida anterior del shell. Usa `app.PrintAbove(element)` (seguro para hilos) para añadir líneas permanentes de registro o chat por encima del prompt activo:

```cpp
auto app = App::TerminalOutput();
app.PrintAbove(paragraph("Build finished!") | color(Color::Green));
```

Consulta [`component/print_above`](/examples/component/print_above).

## Bucles de eventos personalizados (`Loop`)

Si quieres integrar FTXUI en un bucle de juego existente o en un reactor de red, usa [`ftxui::Loop`](/api/class/loop) en lugar de `app.Loop(...)`:

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

Consulta [`component/custom_loop`](/examples/component/custom_loop).

## Restaurar la E/S de la terminal (`WithRestoredIO`)

Para suspender temporalmente el modo raw de la terminal y lanzar un programa externo (como `$EDITOR`, `less` o `git`), envuelve la llamada con `app.WithRestoredIO(...)`:

```cpp
auto edit_btn = Button("Open Editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
```

Consulta [`component/with_restored_io`](/examples/component/with_restored_io).

## Próximos pasos

- [Eventos, foco y ratón](/es/guide/events-and-focus): enrutamiento de eventos, foco y detección de clics de ratón
- [Widgets personalizados](/es/guide/custom-widgets): heredar de `ComponentBase` y `Node`
- [Referencia de la API de `ftxui/component`](/api/component): lista completa de funciones y opciones
