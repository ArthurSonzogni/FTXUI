# Module Screen (`ftxui/screen`)

Le module **Screen** constitue la base bas niveau de FTXUI. Il gère une grille 2D de cellules de terminal stylisées (`Screen`, `Surface`, `Cell`, `Color`, `Box`) ainsi que la détection des capacités du terminal.

## `Screen` et `Cell`

`Screen` représente une grille 2D de `Cell`. Vous pouvez modifier les cellules directement avec `screen.CellAt(x, y)` (qui vérifie les bornes et retourne sans danger une cellule factice en cas de dépassement) :

```cpp
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

int main() {
  using namespace ftxui;

  auto screen = Screen::Create(
      Dimension::Full(),    // Largeur complète du terminal
      Dimension::Fixed(10)  // Hauteur fixe de 10 lignes
  );

  auto& cell = screen.CellAt(10, 5);
  cell.character = "X";
  cell.foreground_color = Color::Red;
  cell.background_color = Color::RGB(0, 255, 0);
  cell.bold = true;

  screen.Print();
}
```

Chaque `Cell` stocke :
- `character` : chaîne de graphème UTF-8
- `foreground_color`, `background_color` : `Color`
- Drapeaux de style : `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlined_double`, `blink`, `strikethrough`

## Affichage et redessin

::: code-group

```cpp [Afficher directement]
screen.Print();
```

```cpp [Convertir en chaîne]
std::cout << screen.ToString();
```

:::

Pour redessiner sur place dans une boucle sans faire défiler le terminal, affichez `screen.ResetPosition()` avant la frame suivante :

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

`Dimension` contrôle le dimensionnement de `Screen::Create(width, height)` :

- `Dimension::Full()` : s'adapte à la largeur ou à la hauteur actuelle du terminal
- `Dimension::Fit(element)` : s'ajuste à la taille minimale requise par un `Element`
- `Dimension::Fixed(n)` : nombre exact de colonnes ou de lignes

## `Color` et capacités du terminal

`Color` prend en charge 4 modes et se replie automatiquement sur la couleur la plus proche supportée par le terminal :

- **Par défaut :** `Color::Default`
- **Palette 16 couleurs :** `Color::Red`, `Color::BlueLight`, … ([démo interactive](/examples/dom/color_gallery))
- **Palette 256 couleurs :** `Color::Palette256`, `Color::Chartreuse1`, … ([démo interactive](/examples/dom/color_info_palette256))
- **TrueColor 24 bits :** `Color::RGB(r, g, b)`, `Color::HSV(h, s, v)` ([démo interactive](/examples/dom/color_truecolor_HSV))

Vous pouvez inspecter ou forcer la prise en charge des couleurs avec `Terminal::ColorSupport()` et `Terminal::SetColorSupport(...)` (voir la [référence API `ftxui/screen`](/api/screen)).
