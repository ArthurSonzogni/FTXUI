@page getting-started Getting Started
@tableofcontents

FTXUI is a functional, C++ library for terminal-based user interfaces. It is organized into three main modules, each building upon the previous one.

# The Three Modules

1.  **Screen**: The lowest level. It handles the terminal's state, colors, and raw character grid.
2.  **DOM**: The layout engine. It provides a set of `Element`s that can be composed to create complex, responsive layouts.
3.  **Component**: The interactive layer. It handles user input (keyboard, mouse) and manages the application's main loop.

# Installation and Dependency

To set up FTXUI in your project, follow the [installation guide](installation.html). 

The most recommended way for CMake users is to use `FetchContent`. Add this to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main # or a specific version like v6.1.9
)
FetchContent_MakeAvailable(ftxui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE ftxui::ftxui)
```

# Your First Static UI (DOM)

The `DOM` module allows you to describe your UI declaratively. Compositing elements is as simple as nesting function calls or using the pipe operator for decorators.

Save this as `main.cpp`:

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

int main() {
  using namespace ftxui;

  // Define the document structure
  Element document = vbox({
    text("FTXUI Getting Started") | bold | center,
    separator(),
    hbox({
      text("Left Panel") | border,
      vbox({
        text("Main Content Area") | flex,
        separator(),
        text("Footer Information") | dim,
      }) | border | flex,
    }) | flex,
  });

  // Create the screen and render
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}
```

# Adding Interactivity (Component)

To handle user input and create a dynamic application, use the `Component` module and the `App` class.

```cpp
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  int counter = 0;

  // Define components
  auto btn_increment = Button("Increment", [&] { counter++; });
  auto btn_decrement = Button("Decrement", [&] { counter--; });
  auto btn_quit = Button("Quit", [] { exit(0); });

  // Compose them into a container
  auto container = Container::Vertical({
    btn_increment,
    btn_decrement,
    btn_quit,
  });

  // Define the renderer
  auto renderer = Renderer(container, [&] {
    return vbox({
      text("Counter: " + std::to_string(counter)) | border,
      separator(),
      container->Render(),
    }) | border;
  });

  // Start the main loop
  auto app = App::TerminalOutput();
  app.Loop(renderer);

  return 0;
}
```

# Next Steps

*   Explore the [Modules](modules.html) documentation for a deeper dive into each layer.
*   Browse the [Examples](https://arthursonzogni.github.io/FTXUI/examples/) to see what's possible.
*   Check out the [Starter Template](https://github.com/ArthurSonzogni/ftxui-starter) for a pre-configured project structure.

<div class="section_buttons">
 
| Previous                          | Next                    |
|:----------------------------------|------------------------:|
| [Introduction](index.html) | [Modules](modules.html) |

 
</div>
