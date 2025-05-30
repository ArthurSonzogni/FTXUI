@page getting-started Getting Started
@tableofcontents

![title-img](https://nsm09.casimages.com/img/2025/05/30//2505300816063242518595256.jpg)

# Install FTXUI

To set up FTXUI in your project, follow the [installation guide](installation.html), which provides instructions for multiple build systems and package managers.

# Minimal Example

Save the following code as `main.cpp`:

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main() {
  using namespace ftxui;

  Element document = hbox({
    text("left")   | border,
    text("middle") | border | flex,
    text("right")  | border,
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
}
```

Build and run it using your preferred build system.  
If unsure, start with one of the methods described in the [installation page](installation.html).

Expected output:

```
┌────┐┌────────────────────────────────────┐┌─────┐
│left││middle                              ││right│
└────┘└────────────────────────────────────┘└─────┘
```

# Starter Template

For a complete working project, clone the official starter repository:

```bash
git clone https://github.com/ArthurSonzogni/ftxui-starter
```

Follow the build instructions in the `README.md` of that repository.

<div class="section_buttons">
 
| Previous                          | Next                    |
|:----------------------------------|------------------------:|
| [Introduction](index.html) | [Modules](modules.html) |

 
</div>
