# Primeros pasos

Esta guía explica cómo añadir FTXUI a un proyecto, renderizar un diseño estático y construir una pequeña aplicación interactiva.

## 1. Añade FTXUI a tu proyecto

::: code-group

```cmake [CMake (FetchContent)]
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3
)
FetchContent_MakeAvailable(ftxui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ftxui::ftxui)
```

```cpp [Single Header]
// Download ftxui_all.hpp from the GitHub release artifacts:
#define FTXUI_IMPLEMENTATION
#include "ftxui_all.hpp"
```

```python [Bazel (MODULE.bazel)]
bazel_dep(name = "ftxui", version = "7.0.3")
```

```ini [Meson (subprojects/ftxui.wrap)]
[wrap-git]
url = https://github.com/ArthurSonzogni/FTXUI.git
revision = v7.0.3
```

```json [vcpkg (vcpkg.json)]
{
  "dependencies": ["ftxui"]
}
```

```ini [Conan (conanfile.txt)]
[requires]
ftxui/7.0.3

[generators]
CMakeDeps
CMakeToolchain
```

:::

¿Necesitas más detalles? Consulta las [guías de instalación](/es/install/) completas.

---

## 2. Diseño estático (DOM)

Construye un documento declarativo con `hbox`, `vbox` y decoradores encadenables (`| border`, `| bold`, `| flex`):

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

int main() {
  using namespace ftxui;

  Element title = text("FTXUI Dashboard") | bold | center;
  Element left_panel = text("Left Panel") | border;
  Element right_panel = vbox({
                            text("Main Content Area") | flex,
                            separator(),
                            text("Status: Ready") | dim,
                        })
                        | border
                        | flex;

  Element document = vbox({
      title,
      separator(),
      hbox({left_panel, right_panel}) | flex,
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
}
```

---

## 3. Añadir interactividad (Component)

Envuelve estado en C++ normal con componentes como `Menu`, `Input` y `Button`, y ejecútalos dentro de `App`:

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  std::vector<std::string> entries = {"Dashboard", "Settings", "Logs"};
  int selected = 0;

  auto menu = Menu(&entries, &selected) | border;

  auto app = App::TerminalOutput();
  app.Loop(menu);
}
```

<FtxuiDemo file="component/menu_style" title="Interactive Menu Demo (click or use arrow keys)" height="260px" />

---

## 4. Componer diseños, atajos y modales

Combina un `Container` (para el enrutamiento de eventos y foco) con `Renderer(container, ...)` (para diseños personalizados) y `CatchEvent` (para atajos de teclado):

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  auto app = App::Fullscreen();
  std::string name;
  bool show_about = false;

  auto input = Input(&name, "Enter your name...");
  auto about_btn = Button("About", [&] { show_about = true; });
  auto quit_btn = Button("Quit", app.ExitLoopClosure());

  auto buttons = Container::Horizontal({about_btn, quit_btn});
  auto controls = Container::Vertical({input, buttons});

  auto main_ui = Renderer(controls, [&] {
    auto greeting =
        text("Hello " + (name.empty() ? "FTXUI" : name) + "!") | bold | hcenter;
    auto input_row = hbox({text(" Name: "), input->Render()});
    auto button_row = hbox({about_btn->Render(), quit_btn->Render()}) | hcenter;

    return vbox({
               greeting,
               separator(),
               input_row,
               separator(),
               button_row,
           })
           | border
           | center;
  });

  auto modal = Button("Close (Esc)", [&] { show_about = false; })
               | border
               | clear_under;

  auto root = main_ui
              | Modal(modal, &show_about)
              | CatchEvent([&](Event e) {
                  if (show_about && e == Event::Escape) {
                    show_about = false;
                    return true;
                  }
                  return false;
                });

  app.Loop(root);
}
```

---

## Próximos pasos

- [Modelo mental](/es/guide/mental-model): cómo funcionan los Elements, Components y el enrutamiento de eventos.
- [Recetario](/es/guide/cookbook): patrones para hilos, desplazamiento, listas dinámicas y modales.
- Referencias de la API de [Elements](/api/dom) y [Components](/api/component).
- Repositorio plantilla [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter), o [`/llms.txt`](/llms.txt) / [`/llms-full.txt`](/llms-full.txt).
