# Módulo Screen (`ftxui/screen`)

El módulo **Screen** es la base de bajo nivel de FTXUI. Gestiona una cuadrícula 2D de celdas de terminal con estilo (`Screen`, `Surface`, `Cell`, `Color`, `Box`) y la detección de capacidades de la terminal.

## `Screen` y `Cell`

`Screen` representa una cuadrícula 2D de objetos `Cell`. Puedes modificar celdas directamente con `screen.CellAt(x, y)` (que comprueba los límites y devuelve de forma segura una celda ficticia si te sales de rango):

```cpp
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

int main() {
  using namespace ftxui;

  auto screen = Screen::Create(
      Dimension::Full(),    // Full terminal width
      Dimension::Fixed(10)  // Fixed height of 10 rows
  );

  auto& cell = screen.CellAt(10, 5);
  cell.character = "X";
  cell.foreground_color = Color::Red;
  cell.background_color = Color::RGB(0, 255, 0);
  cell.bold = true;

  screen.Print();
}
```

Cada `Cell` almacena:
- `character`: cadena UTF-8 con el grafema
- `foreground_color`, `background_color`: `Color`
- Indicadores de estilo: `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlined_double`, `blink`, `strikethrough`

## Impresión y redibujado

::: code-group

```cpp [Imprimir directamente]
screen.Print();
```

```cpp [Convertir a cadena]
std::cout << screen.ToString();
```

:::

Para redibujar en el mismo sitio dentro de un bucle sin desplazar la terminal, imprime `screen.ResetPosition()` antes del siguiente fotograma:

```cpp
std::string reset_position;
while (running) {
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  std::cout << reset_position;
  screen.Print();
  reset_position = screen.ResetPosition();
}
```

## `Dimension`

`Dimension` controla el tamaño en `Screen::Create(width, height)`:

- `Dimension::Full()`: coincide con el ancho o alto actual de la terminal
- `Dimension::Fit(element)`: se ajusta al tamaño mínimo requerido por un `Element`
- `Dimension::Fixed(n)`: número exacto de columnas o filas

## `Color` y capacidades de la terminal

`Color` soporta 4 modos y se adapta automáticamente al color más cercano soportado por la terminal:

- **Predeterminado:** `Color::Default`
- **Paleta de 16 colores:** `Color::Red`, `Color::BlueLight`, … ([demo en vivo](/examples/dom/color_gallery))
- **Paleta de 256 colores:** `Color::Palette256`, `Color::Chartreuse1`, … ([demo en vivo](/examples/dom/color_info_palette256))
- **TrueColor de 24 bits:** `Color::RGB(r, g, b)`, `Color::HSV(h, s, v)` ([demo en vivo](/examples/dom/color_truecolor_HSV))

Puedes inspeccionar o anular el soporte de color con `Terminal::ColorSupport()` y `Terminal::SetColorSupport(...)` (consulta la [Referencia de la API de `ftxui/screen`](/api/screen)).
