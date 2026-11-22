# Módulos de C++20

::: warning Experimental
Esta característica sigue en desarrollo y la API puede cambiar en futuras versiones. Si encuentras algún problema o tienes sugerencias, abre un issue en GitHub.
:::

FTXUI soporta de forma experimental los
[módulos de C++20](https://en.cppreference.com/w/cpp/language/modules) para reducir
los tiempos de compilación y mejorar la organización del código. Cada parte de la biblioteca tiene un
módulo correspondiente, dividido en particiones por cada cabecera.

Usa la opción `FTXUI_BUILD_MODULES` al compilar el propio proyecto FTXUI para proporcionar módulos de C++20,
por ejemplo con CMake y Ninja:

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```

::: info Nota
Para usar módulos necesitas un compilador C++20, CMake 3.20 o superior y un generador compatible como Ninja. Los generadores de Makefile no soportan módulos de C++20.
:::

Después, en tu propio código puedes consumir los módulos con normalidad:

```cpp
import ftxui;

using ftxui::App;
using ftxui::Button;
using ftxui::Component;

int main() {
  App app = App::TerminalOutput();
  Component button = Button("Click me", app.ExitLoopClosure());
  app.Loop(button);
  return 0;
}
```

Escribir `import ftxui;` equivale a incluir todas las cabeceras `<ftxui/**/*.hpp>` y proporciona
la biblioteca completa a través de un único módulo.

Para encontrar y enlazar correctamente los módulos con CMake, usa `target_link_libraries` para obtener las
opciones adecuadas del compilador y del enlazador.

```cmake
target_link_libraries(my_executable
    #...whatever...
    PRIVATE ftxui::modules
)
```

### Lista de módulos

Aunque `import ftxui;` ofrece toda la biblioteca, FTXUI está diseñada por capas. Si solo necesitas funcionalidades concretas, puedes importar directamente los módulos independientes:

- `ftxui` (módulo de conveniencia que reexporta todos los siguientes)
    - `ftxui.component` (componentes interactivos, eventos y bucles de eventos)
    - `ftxui.dom` (diseño y estilos mediante Elements)
    - `ftxui.screen` (renderizado en terminal, celdas y colores)
    - `ftxui.util` (utilidades internas)

Por ejemplo:
```cpp
import ftxui.screen;
import ftxui.dom;

// Use only screen and dom functionalities...
```
