# Module DOM (`ftxui/dom`)

Le module **DOM** définit un arbre déclaratif sans état de nœuds `Element` qui s'adaptent automatiquement à la taille du terminal.

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

using namespace ftxui;

auto title = text("The Window") | bold | color(Color::Blue);
auto progress = gauge(0.5f);
auto footer = text("The Footer") | dim;

auto document = vbox({
                    title,
                    progress,
                    footer,
                })
                | border;
```

Les décorateurs peuvent s'appliquer de trois façons équivalentes :
- Appel de fonction : `document = border(document);`
- Opérateur pipe : `document = document | border;`
- Pipe sur place : `document |= border;`

## Texte et paragraphes

- `text(str)` : texte UTF-8 sur une seule ligne (compatible pleine chasse, CJK et emojis).
- `vtext(str)` : texte vertical (un graphème par ligne).
- `paragraph(str)` : texte multiligne avec retour à la ligne par mots, ainsi que `paragraphAlignLeft`, `paragraphAlignCenter`, `paragraphAlignRight`, `paragraphAlignJustify`.

```cpp
auto body = paragraph("Long text wraps automatically to fit the container width.")
            | borderRounded;
```

Voir les exemples interactifs [`dom/paragraph`](/examples/dom/paragraph) et [`dom/html_like`](/examples/dom/html_like).

## Conteneurs de mise en page

| Fonction | Mise en page |
| --- | --- |
| `hbox({...})` | Ligne horizontale |
| `vbox({...})` | Colonne verticale |
| `dbox({...})` | Calques empilés en profondeur (à associer avec `clear_under`) |
| `flexbox({...}, config)` | Mise en page à retour à la ligne de type CSS (raccourcis `hflow` / `vflow`) |
| `gridbox({ {...}, {...} })` | Grille 2D alignée |
| `filler()` | Espace vide extensible |

Utilisez `flex` (ou `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `size(WIDTH, EQUAL, n)`) pour contrôler la répartition de l'espace disponible entre les éléments :

```cpp
auto left = text("left") | border;
auto middle = text("middle") | border | flex;
auto right = text("right") | border;

auto row = hbox({left, middle, right});
```

```text
┌────┐┌─────────────────────────────────────────────────────┐┌─────┐
│left││middle                                               ││right│
└────┘└─────────────────────────────────────────────────────┘└─────┘
```

Explorez les démos de mise en page : [`dom/vbox_hbox`](/examples/dom/vbox_hbox), [`dom/gridbox`](/examples/dom/gridbox), [`dom/hflow`](/examples/dom/hflow), [`dom/dbox`](/examples/dom/dbox), [`dom/size`](/examples/dom/size).

## Bordures, fenêtres et séparateurs

- **Bordures :** `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(BorderStyle, Color)`.
- **`window(title, content)` :** boîte encadrée avec un titre intégré dans le bord supérieur.
- **Séparateurs :** `separator()`, `separatorLight()`, `separatorDouble()`, `separatorHeavy()`, `separatorDashed()`, `separatorEmpty()`, `separatorCharacter(str)`.

```cpp
auto card = window(text(" Status ") | bold,
                   vbox({
                       text("CPU: 12%"),
                       separator(),
                       text("RAM: 4.2 GB"),
                   }));
```

Voir [`dom/border_style`](/examples/dom/border_style), [`dom/border_colored`](/examples/dom/border_colored) et [`dom/separator_style`](/examples/dom/separator_style).

## Couleurs, dégradés et styles de texte

FTXUI prend en charge les palettes 16 couleurs (`Color::Red`, `Color::BlueLight`, …), 256 couleurs (`Color::Palette256`) et TrueColor 24 bits (`Color::RGB(r, g, b)`, `Color::HSV(h, s, v)`), avec repli automatique sur les terminaux aux capacités réduites.

```cpp
auto styled = text("Hello FTXUI")
              | bold
              | underlined
              | color(Color::RGB(120, 220, 255))
              | bgcolor(Color::RGB(20, 25, 40));

auto gradient_box = text("Gradient")
                    | center
                    | bgcolor(LinearGradient(45, Color::DeepPink1, Color::DodgerBlue1));
```

<FtxuiDemo file="dom/color_gallery" height="440px" title="Live demo: dom/color_gallery" />

Décorateurs de style de texte : `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough`, `hyperlink(url)`.

Voir [`dom/style_gallery`](/examples/dom/style_gallery), [`dom/linear_gradient`](/examples/dom/linear_gradient) et [`dom/color_info_palette256`](/examples/dom/color_info_palette256).

## Jauges, indicateurs de chargement et graphiques

- `gauge(progress)` (de `0.0f` à `1.0f`), ainsi que `gaugeLeft`, `gaugeRight`, `gaugeUp`, `gaugeDown`.
- `spinner(charset_index, frame)` : 23 styles de spinners animés intégrés.
- `graph(fn)` : rend un callback `std::vector<int>(int width, int height)` sous forme de courbe en caractères Braille.

Voir [`dom/gauge`](/examples/dom/gauge), [`dom/gauge_direction`](/examples/dom/gauge_direction), [`dom/spinner`](/examples/dom/spinner) et [`dom/graph`](/examples/dom/graph).

## Tableaux

Incluez `<ftxui/dom/table.hpp>` pour construire des tableaux alignés et styler des lignes, des colonnes ou des rectangles avec `TableSelection` :

```cpp
#include <ftxui/dom/table.hpp>

auto table = Table({
    {"Language", "Stars", "License"},
    {"C++",      "10k+",  "MIT"},
    {"FTXUI",    "Fast",  "MIT"},
});

table.SelectAll().Border(LIGHT);
table.SelectRow(0).Decorate(bold);
table.SelectRow(0).SeparatorVertical(LIGHT);
table.SelectRow(0).Border(DOUBLE);
table.SelectColumn(1).DecorateCells(align_right);

auto doc = table.Render();
```

Voir [`dom/table`](/examples/dom/table) pour un aperçu complet des styles.

## Canvas

Incluez `<ftxui/dom/canvas.hpp>` pour le dessin sous-cellulaire en pixels, Braille ou blocs (lignes, cercles, texte, points) :

```cpp
#include <ftxui/dom/canvas.hpp>

auto c = Canvas(80, 40);
c.DrawPointCircle(40, 20, 15, Color::Cyan);
c.DrawPointLine(0, 0, 80, 40, Color::Yellow);
c.DrawText(28, 20, "FTXUI", Color::White);

auto doc = canvas(std::move(c)) | border;
```

Voir [`dom/canvas`](/examples/dom/canvas) et [`component/canvas_animated`](/examples/component/canvas_animated).

## Défilement, réflexion et superposition

- `frame`, `xframe`, `yframe` : recadrent un élément dans une zone visible qui défile jusqu'à l'enfant portant `focus`. À associer avec `vscroll_indicator` / `hscroll_indicator`.
- `reflect(box)` : enregistre les coordonnées finales de l'élément à l'écran dans une `Box` (utile pour la détection de clic souris).
- `clear_under` : efface les cellules situées sous une couche superposée dans une `dbox`.
- `automerge` : fusionne les caractères de bordure adjacents (`─`, `│`, `┌`, `┼`).

Pour la liste complète des fonctions DOM et leurs signatures, consultez la [référence API `ftxui/dom`](/api/dom).
