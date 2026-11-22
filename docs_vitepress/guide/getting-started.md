# Getting Started

This guide walks through adding FTXUI to a project, rendering a static layout, and building a small interactive application.

## 1. Add FTXUI to Your Project

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

Need more details? See the full [Installation guides](/install/).

---

## 2. Static Layout (DOM)

Build a declarative document with `hbox`, `vbox`, and pipeable decorators (`| border`, `| bold`, `| flex`):

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

## 3. Adding Interactivity (Component)

Wrap plain C++ state with components like `Menu`, `Input`, and `Button`, and run them inside `App`:

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

## 4. Composing Layout, Shortcuts, and Modals

Combine a `Container` (for event/focus routing) with `Renderer(container, ...)` (for custom layout) and `CatchEvent` (for keyboard shortcuts):

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

## Next Steps

- [Mental Model](/guide/mental-model): how Elements, Components, and event routing work.
- [Cookbook](/guide/cookbook): patterns for threads, scrolling, dynamic lists, and modals.
- [Elements](/api/dom) and [Components](/api/component) API references.
- [ftxui-starter](https://github.com/ArthurSonzogni/ftxui-starter) template repository, or [`/llms.txt`](/llms.txt) / [`/llms-full.txt`](/llms-full.txt).
