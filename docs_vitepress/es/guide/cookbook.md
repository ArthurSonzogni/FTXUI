# Recetario

Patrones habituales para construir aplicaciones con FTXUI.

## 1. Hilos en segundo plano y actualizaciones en vivo

Nunca modifiques el estado de la interfaz directamente desde un hilo en segundo plano mientras `App::Loop()` está renderizando. En su lugar, usa `App::Post()` (seguro para hilos) para ejecutar las actualizaciones de estado en el hilo del bucle principal, y `App::RequestAnimationFrame()` para programar un redibujado:

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

## 2. Listas dinámicas de componentes

Cuando un `std::vector` de elementos crece o se reduce, reconstruye los hijos del contenedor con `DetachAllChildren()` y `Add(...)` (siempre en el hilo del bucle principal, por ejemplo dentro del callback de un botón o de `app.Post`):

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

## 3. Panel desplazable con rueda del ratón

`vscroll_indicator` es un indicador visual pasivo. Para que el usuario pueda desplazar un panel con la rueda del ratón y el teclado, combina `yframe` + `vscroll_indicator` con `focusPositionRelative` y `CatchEvent`:

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

## 4. Atajos globales y `Ctrl+C` personalizado

Adjunta `CatchEvent` al componente raíz para definir atajos globales. Devuelve `true` para consumir el evento o `false` para dejar que se propague a los hijos:

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

## 5. Modales y diálogos de confirmación

Envuelve tu componente principal con `Modal(dialog, &show_modal)`. Añade `CatchEvent` al diálogo para cerrarlo al pulsar `Escape`:

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

## 6. Detección de clics y coordenadas relativas al elemento

Captura el rectángulo delimitador `Box` renderizado de un elemento con `| reflect(box)` durante `Render()` y luego comprueba los clics dentro de `CatchEvent`:

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

## 7. Salida CLI continua y lanzamiento de editores externos

Usa `App::TerminalOutput()` para CLIs en línea, `PrintAbove()` para imprimir líneas permanentes en el historial de la terminal y `WithRestoredIO()` para devolver temporalmente la terminal a un programa externo:

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
