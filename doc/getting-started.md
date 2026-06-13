@page getting-started Getting Started
@tableofcontents

![title-img](https://nsm09.casimages.com/img/2025/05/30//2505300816063242518595256.jpg)

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
  GIT_TAG main # or a specific version like v7.0.0
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

To handle user input and create a dynamic application, use the `Component` module and the `App` class. Components manage their own state and can be composed using containers.

```cpp
#include <ftxui/component/component.hpp>
#include <ftxui/component/app.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  std::vector<std::string> entries = {
      "Entry 1",
      "Entry 2",
      "Entry 3",
  };
  int selected = 0;

  // Create a menu component
  auto menu = Menu(&entries, &selected);

  // You can decorate components using the pipe operator.
  auto component = menu | border;

  // Start the main loop
  auto app = App::TerminalOutput();
  app.Loop(component);

  return 0;
}
```

# Next Steps

*   Browse the [Examples](https://arthursonzogni.github.io/FTXUI/examples/) to see what's possible.
*   Check out the [Starter Template](https://github.com/ArthurSonzogni/ftxui-starter) for a pre-configured project structure.

<div class="section_buttons">
 
| Previous                          | Next                    |
|:----------------------------------|------------------------:|
| [Introduction](index.html) | [Modules](modules.html) |

 
</div>
