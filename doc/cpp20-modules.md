@page cpp20-modules C++20 Modules


> [!WARNING]
> This feature is still in development, and the API may change in future releases.
> Your contribution is needed to help us improve the compatibility and usability
> of C++20 modules in FTXUI. If you encounter any issues or have suggestions,
> please open an issue.

FTXUI experimentally supports
[C++20 modules](https://en.cppreference.com/w/cpp/language/modules) to reduce
compilation times and improve code organization. Each header has a
corresponding module.

**Example with CMake and Ninja**


```cpp
import ftxui;

int main() {
  auto screen = ftxui::ScreenInteractive::TerminalOutput();
  auto button = ftxui::Button("Click me", screen.QuitClosure());
  screen.Loop(button);
  return 0;
}
```

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```
> [!NOTE]
> To use modules, you need a C++20 compatible compiler, CMake version 3.20 or
> higher, and use a compatible generator like Ninja. Note that Makefile
> generators **do not support modules**.

### Module list

The modules directly reference the corresponding header, or a group of related
headers to provide a more convenient interface. The following modules 
are available:

- `ftxui`
    - `ftxui.component`
      - `ftxui.component.Animation`
      - `ftxui.component.CapturedMouse`
      - `ftxui.component.Component`
      - `ftxui.component.ComponentBase`
      - `ftxui.component.ComponentOptions`
      - `ftxui.component.Event`
      - `ftxui.component.Loop`
      - `ftxui.component.Mouse`
      - `ftxui.component.Receiver`
      - `ftxui.component.ScreenInteractive`
      - `ftxui.component.Task`
- `ftxui.dom`
    - `ftxui.dom.Canvas`
    - `ftxui.dom.Deprecated`
    - `ftxui.dom.Direction`
    - `ftxui.dom.Elements`
    - `ftxui.dom.FlexboxConfig`
    - `ftxui.dom.LinearGradient`
    - `ftxui.dom.Node`
    - `ftxui.dom.Requirement`
    - `ftxui.dom.Selection`
    - `ftxui.dom.Table`
- `ftxui.screen`
    - `ftxui.screen.Box`
    - `ftxui.screen.Color`
    - `ftxui.screen.ColorInfo`
    - `ftxui.screen.Deprecated`
    - `ftxui.screen.Image`
    - `ftxui.screen.Pixel`
    - `ftxui.screen.Screen`
    - `ftxui.screen.String`
    - `ftxui.screen.Terminal`
- `ftxui.util`
    - `ftxui.util.AutoReset`
    - `ftxui.util.Ref`
